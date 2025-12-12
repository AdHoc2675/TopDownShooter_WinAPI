#pragma once
#include "CPanel.h"

class CPlayer;

enum class UpgradeType
{
	AtkUp,                  // 공격력 +10%
	MaxHpUpHeal,            // 최대 체력 +2 및 즉시 +2 회복
	CritChanceUp,           // 치명타 확률 +0.05
	SpdUp,                  // 이동 속도 +0.05
	CritDmgUp,              // 치명타 피해 배율 +0.1
	SummonRanged,           // 꼬마 혼령
	SummonScythe,           // 회전 낫
	WeaponDoubleShot_T1,    // 더블샷
	WeaponQuickHands_T1,    // 빠른 손
	WeaponRapidFire_T1,     // 속사
	WeaponPenetration,      // 관통탄
	WeaponArmourPiercing,   // 대전차탄
	WeaponAimingDownSight,  // 정조준
	WeaponBurn,             // 발화탄
	ScytheSpeedUp,          // 낫 회전 속도 2배
	AimingDownSight,        // 치명타 확률 +20%
    SummonRangedBurn,       // 소환수의 투사체에 관통 +1, 화상 1스택 부여
    SummonRangedAtkUp,      // 소환수 투사체의 기본 피해량 += 관통 * 3
    WeaponDamageBoostOnReload,   // 재장전 시 다음 N발 피해 증가
	WeaponSpawnVolleyOnReload,   // 재장전 시 주변에 투사체 발사

};

class CUpgradePanel : public CPanel
{
public:
    CUpgradePanel();
    virtual ~CUpgradePanel();

    // 플레이어와 선택지 구성
    void Configure(CPlayer* player);

private:
    void Init() override;
    void OnEnable() override;
    void Update() override;
    void Render() override;
    void OnDisable() override;
    void Release() override;

    // 버튼 클릭 콜백(static → param으로 this 전달)
    static void OnButtonClicked(DWORD_PTR param1, DWORD_PTR param2);
    void ApplyUpgrade(UpgradeType type);

    CPlayer* player;
    struct Option { wstring label; UpgradeType type; Vec2 btnPos; Vec2 btnSize; };
    vector<Option> options;
	bool buttonsCreated;
};