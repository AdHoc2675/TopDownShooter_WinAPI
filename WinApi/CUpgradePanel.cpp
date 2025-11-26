#include "pch.h"
#include "CUpgradePanel.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CButton.h"
#include "CCombatSystem.h"
    
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

    // 버튼 생성 (패널의 자식 UI로 추가)
    for (const auto& o : options)
    {
        CButton* btn = new CButton();
        btn->SetName(TEXT("UpgradeButton"));
        btn->SetPos(o.btnPos);      // 패널 기준 상대 좌표
        btn->SetScale(o.btnSize);
        btn->SetClickCallback(&CUpgradePanel::OnButtonClicked,
                              (DWORD_PTR)this, (DWORD_PTR)o.type);
        EVENT->AddChild(this, btn);
    }
}

void CUpgradePanel::Update()
{
    //CPanel::Update();
}

void CUpgradePanel::Render()
{
    // 패널 배경
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(240, 240, 240));
    RENDER->Rect(renderPos.x, renderPos.y, renderPos.x + scale.x, renderPos.y + scale.y);

    // 제목
    int titleSize = 28;
    RENDER->SetText(titleSize, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(renderPos.x + scale.x * 0.5f, renderPos.y + 20.f, L"Level Up!");

    // 옵션 라벨
    int labelSize = 18;
    RENDER->SetText(labelSize, RGB(20, 20, 20), TextAlign::Left);
    RENDER->SetTextBackMode(TextBackMode::Null);
    for (const auto& opt : options)
    {
        float tx = renderPos.x + opt.btnPos.x + 14.f;
        float ty = renderPos.y + opt.btnPos.y + opt.btnSize.y * 0.5f - labelSize * 0.5f;
        RENDER->Text(tx, ty, opt.label);
    }
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

    const Vec2 btnSize(520.f, 56.f);
    const float startX = 40.f;
    const float startY = 90.f;
    const float gap = 70.f;

    vector<pair<wstring, UpgradeType>> defs = {
        { L"공격력 +5",                UpgradeType::AtkUp },
        { L"최대 체력 +2 및 즉시 +2 회복", UpgradeType::MaxHpUpHeal },
        { L"치명타 확률 +10%",         UpgradeType::CritChanceUp },
    };

    for (int i = 0; i < (int)defs.size(); ++i)
    {
        Option o;
        o.label   = defs[i].first;
        o.type    = defs[i].second;
        o.btnPos  = Vec2(startX, startY + i * gap);
        o.btnSize = btnSize;
        options.push_back(o);
    }
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
    default:
        break;
    }

    // 패널 닫고 일시정지 해제
    if (GetScene())
    {
        GetScene()->SetPaused(false);
        EVENT->Delete(GetScene(), this);
    }
}