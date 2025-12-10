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

    void RenderWeaponDescription();  // 하단 설명란 렌더링
    void StartGame();                // 게임 시작 처리

private:
    int selected = 0;      // 현재 선택된 무기 인덱스
    int hoveredIndex = -1; // 마우스 호버 중인 박스 인덱스

    // 무기 박스 영역 (마우스 클릭 감지용)
    struct BoxRect {
        float left, top, right, bottom;
    };
    BoxRect weaponBoxes[3];

    // 무기 이미지
    CImage* pistolImage = nullptr;
    CImage* shotgunImage = nullptr;
    CImage* smgImage = nullptr;
};

