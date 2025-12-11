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
};

