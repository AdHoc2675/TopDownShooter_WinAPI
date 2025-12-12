#pragma once
#include "CScene.h"
#include "CAnimator.h"

class CSceneTitle : public CScene
{
public:
    CSceneTitle();
    virtual ~CSceneTitle();

    void Init()   override;
    void Enter()  override;
    void Update() override;
    void Render() override;
    void Exit()   override;
    void Release() override;

    void RenderWeaponDescription();
    void RenderCharacterDescription();
    void StartGame();

private:
    // 무기 선택
    int selectedWeapon = 0;
    int hoveredWeaponIndex = -1;

    // 캐릭터 선택 추가
    int selectedCharacter = 0;
    int hoveredCharacterIndex = -1;

    struct BoxRect {
        float left, top, right, bottom;
    };
    BoxRect weaponBoxes[3];
    BoxRect characterBoxes[2];  // 캐릭터 2개

    // 무기 이미지
    CImage* pistolImage = nullptr;
    CImage* shotgunImage = nullptr;
    CImage* smgImage = nullptr;

    // 캐릭터 이미지
    CImage* shanaImage = nullptr;
    CImage* diamondImage = nullptr;

	CSound* UIClickSound = nullptr;

    // --- 타이틀 업그레이드 UI ---
    // 업그레이드 저장
    void SaveTitleUpgrades();
    void LoadTitleUpgrades();

    // 업그레이드 상태 (영구 저장)
    int dummyUpgradeCount = 0;   // 재화 소모용 (무한)
    int speedUpgradeLevel = 0;   // 이동속도 증가(0..3)

    // UI 영역
    BoxRect upgradeDummyBox;     // 재화 소모용 업그레이드 박스
    BoxRect upgradeSpeedBox;     // 이동속도 업그레이드 박스

    // 입력 홀드(1초) 처리
    bool   isHolding = false;
    int    holdingIndex = -1;    // 0 = dummy, 1 = speed
    float  holdTimer = 0.f;
    bool   purchasedThisHold = false;

    // 비용 상수
    static const int kDummyCost = 500;
    int speedCosts[3] = { 1000, 2000, 3000 };

    // 렌더 보조
    void RenderUpgradeUI();
};

