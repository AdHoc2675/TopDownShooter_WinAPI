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
    ScytheSpeedUp,

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