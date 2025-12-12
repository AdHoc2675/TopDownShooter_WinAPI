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
#include "CWeapon.h"

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
        btn->SetLabelSize(12);
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
        { L"꼬마 혼령: 관통 3과 공격력 10의 투사체 발사", UpgradeType::SummonRanged },
		{ L"회전 낫: 플레이어 주위에서 피해 12를 주는 낫 2체 소환", UpgradeType::SummonScythe },
        { L"더블샷: 투사체 수 +1, 산탄각 +30%, 피해량 -10%", UpgradeType::WeaponDoubleShot_T1 },
        { L"빠른 손: 재장전 속도 -20%, 공격 속도 +5%", UpgradeType::WeaponQuickHands_T1 },
        { L"속사: 공격 속도 +25%",                UpgradeType::WeaponRapidFire_T1 },
        { L"관통탄: 관통 횟수 +1, 탄환 속도 +15%", UpgradeType::WeaponPenetration},
        { L"정조준: 치명타 확률 +20%", UpgradeType::AimingDownSight},
        { L"발화탄: 30초 확률로 적에게 발화 부여 (초당 3 피해, 5초, 최대 20중첩)", UpgradeType::WeaponBurn },
        { L"강화탄창: 재장전 완료 시 다음 3발 피해량 25% 증가", UpgradeType::WeaponDamageBoostOnReload },
    };

    // 반복 가능한 옵션
    vector<pair<wstring, UpgradeType>> repeatablePool = {
        { L"(반복) 공격력 +10% (최소 1)",                      UpgradeType::AtkUp },
        { L"(반복) 최대 체력 +2 및 즉시 +2 회복",    UpgradeType::MaxHpUpHeal },
        { L"(반복) 치명타 확률 +5%",               UpgradeType::CritChanceUp },
        { L"(반복) 이동 속도 +5%",                 UpgradeType::SpdUp },
        { L"(반복) 치명타 피해 배율 +0.1",              UpgradeType::CritDmgUp },
    };

    // 조건부 옵션: SummonScythe를 이미 보유한 경우에만 추가
    if (gTakenOneTimeUpgrades.find(UpgradeType::SummonScythe) != gTakenOneTimeUpgrades.end())
    {
        oneTimePool.push_back({ L"낫 강화: 회전 속도 2배", UpgradeType::ScytheSpeedUp });
    }

    if (gTakenOneTimeUpgrades.find(UpgradeType::WeaponPenetration) != gTakenOneTimeUpgrades.end())
    {
        oneTimePool.push_back({ L"대전차탄: 피해량 + 25%, 관통 횟수 +2, 탄환 속도 -15%, 탄창 크기 -25%", UpgradeType::WeaponArmourPiercing });
	}

    if (gTakenOneTimeUpgrades.find(UpgradeType::SummonRanged) != gTakenOneTimeUpgrades.end())
    {
        oneTimePool.push_back({ L"소환수 화염탄: 소환수 투사체 관통 +1 및 화상 1스택 부여", UpgradeType::SummonRangedBurn });
        oneTimePool.push_back({ L"소환수 공격 강화: 관통 * 3만큼 투사체 피해 증가", UpgradeType::SummonRangedAtkUp });
    }


    if (gTakenOneTimeUpgrades.find(UpgradeType::WeaponDamageBoostOnReload) != gTakenOneTimeUpgrades.end())
    {
        oneTimePool.push_back({ L"난사: 재장전 완료 시 전 방향으로 6발 발사", UpgradeType::WeaponSpawnVolleyOnReload });
	}

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
	float increase = 0.f;
    switch (type)
    {
    case UpgradeType::AtkUp:
        // 10% 증가량 계산
        increase = s.attack * 0.1f;

        // 최소 1 보장
        if (increase < 1.f)
            increase = 1.f;

        s.attack += increase;
        break;
    case UpgradeType::MaxHpUpHeal:
        s.maxHp += 2.f;
        s.hp = min(s.hp + 2.f, s.maxHp);
        break;
    case UpgradeType::CritChanceUp:
        s.critChance = min(1.f, s.critChance + 0.05f);
        break;
    case UpgradeType::SpdUp:
        s.speed = s.speed * 1.05f;
        break;
    case UpgradeType::CritDmgUp:
        s.critMultiplier += 0.1f;
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
    case UpgradeType::SummonRangedBurn:
    {
        // 한 번만 획득
        gTakenOneTimeUpgrades.insert(UpgradeType::SummonRangedBurn);

        // 플레이어 소유의 모든 CMissileTurret에 적용
        for (CMissileTurret* turret : CMissileTurret::GetAll())
        {
            if (turret && turret->GetOwnerPlayer() == player)
            {
                turret->AddPierce(1);               // 관통 +1
                turret->AddBurn(1);                 // 화상 스택 +1
            }
        }
        break;
    }
    case UpgradeType::SummonRangedAtkUp:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::SummonRangedAtkUp);

        // 플레이어 소유의 모든 CMissileTurret에 적용
        for (CMissileTurret* turret : CMissileTurret::GetAll())
        {
            if (turret && turret->GetOwnerPlayer() == player)
            {
                turret->EnablePierceDamageBonus(); // 투사체 피해 += 관통 * 3
            }
        }
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
    case UpgradeType::ScytheSpeedUp:
    {
        // 모든 활성 낫에 일괄 적용
        for (CScythe* scythe : CScythe::GetAll())
        {
            if (scythe) scythe->SetAngularSpeed(scythe->GetAngularSpeed() * 2.0f);
        }
        gTakenOneTimeUpgrades.insert(UpgradeType::ScytheSpeedUp);
        break;
    }
    case UpgradeType::WeaponDoubleShot_T1:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponDoubleShot_T1);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_DoubleShot_T1();
        break;
	}
    case UpgradeType::WeaponQuickHands_T1:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponQuickHands_T1);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_QuickHands_T1();
        break;
    }
    case UpgradeType::WeaponRapidFire_T1:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponRapidFire_T1);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_RapidFire_T1();
        break;
	}
    case UpgradeType::WeaponPenetration:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponPenetration);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_Penetration();
        break;
	}
    case UpgradeType::WeaponArmourPiercing:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponArmourPiercing);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_ArmourPiercing();
        break;
	}
    case UpgradeType::WeaponBurn:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponBurn);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_Burn();
        break;
    }
    case UpgradeType::AimingDownSight:
        s.critChance = min(1.f, s.critChance + 0.20f);
        gTakenOneTimeUpgrades.insert(UpgradeType::AimingDownSight);
		break;
    case UpgradeType::WeaponDamageBoostOnReload:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponDamageBoostOnReload);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_DamageBoostOnReload();
        break;
    }
    case UpgradeType::WeaponSpawnVolleyOnReload:
    {
        gTakenOneTimeUpgrades.insert(UpgradeType::WeaponSpawnVolleyOnReload);
        CWeapon* w = player->GetWeapon();
        if (w)
            w->ApplyUpgrade_SpawnVolleyOnReload();
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