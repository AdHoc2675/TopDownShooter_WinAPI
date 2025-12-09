#pragma once
#include "CPanel.h"

class CPlayer;

enum class UpgradeType
{
    AtkUp,
    MaxHpUpHeal,
    CritChanceUp,
    SpdUp,
    CritDmgUp,
    SummonRanged,
    SummonScythe,
	WeaponDoubleShot_T1,
	WeaponQuickHands_T1,
	WeaponRapidFire_T1,
	WeaponPenetration,
    WeaponArmourPiercing,
    WeaponAimingDownSight,
    WeaponBurn,
    ScytheSpeedUp,
    AimingDownSight,
    SummonRangedBurn,  // 소환수의 투사체에 관통 +1, 화상 1스택 부여
    SummonRangedAtkUp,  // 소환수 투사체의 기본 피해량 += 관통 * 3

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