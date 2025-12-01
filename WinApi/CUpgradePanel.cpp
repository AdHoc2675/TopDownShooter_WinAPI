#include "pch.h"
#include <algorithm>
#include <random>
#include <unordered_set>

#include "CUpgradePanel.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CButton.h"
#include "CCombatSystem.h"
#include "CIconTextButton.h"
#include "CResourceManager.h"
#include "CMissileTurret.h"
#include "CScythe.h"

using namespace std;

// 반복 불가능(한 번만 선택 가능한) 업그레이드의 획득 상태를 전역적으로 기록
static unordered_set<UpgradeType> gTakenOneTimeUpgrades;

CUpgradePanel::CUpgradePanel()
{
    player = nullptr;
    buttonsCreated = false;
    SetName(TEXT("UpgradePanel"));
    SetScreenFixed(true);
}

CUpgradePanel::~CUpgradePanel()
{
}

void CUpgradePanel::Init()
{
    const float panelW = 600.f;
    const float panelH = 360.f;
    SetScale(Vec2(panelW, panelH));
    SetPos(Vec2(CGame::WINSIZE.x * 0.5f - panelW * 0.5f,
                CGame::WINSIZE.y * 0.5f - panelH * 0.5f));
}

void CUpgradePanel::OnEnable()
{
    if (buttonsCreated) return;
    buttonsCreated = true;

    for (const auto& o : options)
    {
        auto* btn = new CIconTextButton();
        btn->SetName(TEXT("UpgradeButton"));
        btn->SetPos(o.btnPos);
        btn->SetScale(o.btnSize);
        btn->SetClickCallback(&CUpgradePanel::OnButtonClicked,
                              (DWORD_PTR)this, (DWORD_PTR)o.type);

        btn->SetLabel(o.label);
        btn->SetLabelSize(18);
        btn->SetLabelColor(RGB(20, 20, 20));

        CImage* icon = nullptr;
        if (o.type == UpgradeType::AtkUp)
            icon = LOADIMAGE(TEXT("IconAtk"), TEXT("Image\\IconAtkUp.bmp"));
        else if (o.type == UpgradeType::MaxHpUpHeal)
            icon = LOADIMAGE(TEXT("IconHP"), TEXT("Image\\IconMaxHpUpHeal.bmp"));
        else if (o.type == UpgradeType::CritChanceUp)
            icon = LOADIMAGE(TEXT("IconCrit"), TEXT("Image\\IconCritChanceUp.bmp"));
        else if (o.type == UpgradeType::SummonRanged)
            icon = LOADIMAGE(TEXT("IconSummon"), TEXT("Image\\IconSummonRanged.bmp"));

        if (icon) {
            btn->SetIcon(icon);
            btn->SetIconTransparent(RGB(255,0,255));
        }

        EVENT->AddChild(this, btn);
    }
}

void CUpgradePanel::Update()
{
    //CPanel::Update();
}

void CUpgradePanel::Render()
{
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(240, 240, 240));
    RENDER->Rect(renderPos.x, renderPos.y, renderPos.x + scale.x, renderPos.y + scale.y);

    int titleSize = 28;
    RENDER->SetText(titleSize, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(renderPos.x + scale.x * 0.5f, renderPos.y + 20.f, L"Level Up!");
}

void CUpgradePanel::OnDisable()
{
}

void CUpgradePanel::Release()
{
}

void CUpgradePanel::Configure(CPlayer* p)
{
    player = p;
    options.clear();

#pragma region 업그레이드 옵션 풀 정의
    // 반복 불가능 옵션
    vector<pair<wstring, UpgradeType>> oneTimePool = {
        { L"소환수: 원거리 지원",            UpgradeType::SummonRanged },
		{ L"소환수: 회전 낫",                UpgradeType::SummonScythe },
    };

    // 반복 가능한 옵션
    vector<pair<wstring, UpgradeType>> repeatablePool = {
        { L"공격력 +5",                      UpgradeType::AtkUp },
        { L"최대 체력 +2 및 즉시 +2 회복",   UpgradeType::MaxHpUpHeal },
        { L"치명타 확률 +10%",               UpgradeType::CritChanceUp },
        { L"이동 속도 +10%",                 UpgradeType::SpdUp },
        { L"치명타 배수 +0.25",              UpgradeType::CritDmgUp },
    };

    // 아직 획득하지 않은 반복 불가능 옵션만 추림
    vector<pair<wstring, UpgradeType>> oneTimeCandidates;
    oneTimeCandidates.reserve(oneTimePool.size());
    for (auto& kv : oneTimePool) {
        if (gTakenOneTimeUpgrades.find(kv.second) == gTakenOneTimeUpgrades.end()) {
            oneTimeCandidates.push_back(kv);
        }
    }

#pragma endregion

    const Vec2 btnSize(520.f, 56.f);
    const float startX = 40.f;
    const float startY = 90.f;
    const float gap = 70.f;

    // 셔플
    random_device rd;
    mt19937 gen(rd());
    shuffle(oneTimeCandidates.begin(), oneTimeCandidates.end(), gen);
    shuffle(repeatablePool.begin(), repeatablePool.end(), gen);

    const size_t pickCount = 3;
    vector<pair<wstring, UpgradeType>> picked;
    picked.reserve(pickCount);

    // 1) 반복 불가능 옵션을 가능한 한 많이 먼저 채움
    for (size_t i = 0; i < oneTimeCandidates.size() && picked.size() < pickCount; ++i) {
        picked.push_back(oneTimeCandidates[i]);
    }

    // 2) 부족하면 반복 가능한 옵션에서 채움
    for (size_t i = 0; i < repeatablePool.size() && picked.size() < pickCount; ++i) {
        picked.push_back(repeatablePool[i]);
    }

    // 버튼 배치
    for (size_t i = 0; i < picked.size(); ++i)
    {
        const auto& def = picked[i];

        Option o;
        o.label  = def.first;
        o.type   = def.second;
        o.btnPos = Vec2(startX, startY + (float)i * gap);
        o.btnSize= btnSize;
        options.push_back(o);
    }

    buttonsCreated = false;
}

void CUpgradePanel::OnButtonClicked(DWORD_PTR param1, DWORD_PTR param2)
{
    CUpgradePanel* panel = reinterpret_cast<CUpgradePanel*>(param1);
    UpgradeType type = static_cast<UpgradeType>(param2);
    if (panel)
        panel->ApplyUpgrade(type);
}

void CUpgradePanel::ApplyUpgrade(UpgradeType type)
{
    if (!player) return;

    CombatStats& s = player->GetCombatStats();
    switch (type)
    {
    case UpgradeType::AtkUp:
        s.attack += 5.f;
        break;
    case UpgradeType::MaxHpUpHeal:
        s.maxHp += 2.f;
        s.hp = min(s.hp + 2.f, s.maxHp);
        break;
    case UpgradeType::CritChanceUp:
        s.critChance = min(1.f, s.critChance + 0.10f);
        break;
    case UpgradeType::SpdUp:
        s.speed = s.speed * 1.1f;
        break;
    case UpgradeType::CritDmgUp:
        s.critMultiplier += 0.25f;
        break;
    case UpgradeType::SummonRanged:
    {
        // 반복 불가능 옵션: 획득 상태 기록
        gTakenOneTimeUpgrades.insert(UpgradeType::SummonRanged);

        CMissileTurret* ally = new CMissileTurret();
        ally->SetPos(player->GetWorldPos() + Vec2(60.f, 0.f));
        ally->SetOwnerPlayer(player);

        ally->GetCombatStats().attack         = 10.f;
        ally->GetCombatStats().defense        = s.defense;
        ally->GetCombatStats().critChance     = 0.f;
        ally->GetCombatStats().critMultiplier = 1.0f;

        EVENT->AddGameObject(GetScene(), ally);
        break;
    }
    case UpgradeType::SummonScythe:
    {
        // 반복 불가능 옵션: 획득 상태 기록
        gTakenOneTimeUpgrades.insert(UpgradeType::SummonScythe);

        CScythe* s1 = new CScythe();
        CScythe* s2 = new CScythe();
        s1->SetOwnerPlayer(player);
        s2->SetOwnerPlayer(player);
        s1->SetInitialAngle(0.f);
        s2->SetInitialAngle(3.141592f); // 반대편
        EVENT->AddGameObject(GetScene(), s1);
        EVENT->AddGameObject(GetScene(), s2);
        break;
	}
    default:
        break;
    }

    if (GetScene())
    {
        GetScene()->SetPaused(false);
        EVENT->Delete(GetScene(), this);
    }
}