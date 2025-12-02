#pragma once
#include "CGameObject.h"

class CPlayer;

class CTiledBackground : public CGameObject
{
public:
    CTiledBackground();
    ~CTiledBackground() override {}

private:
    void Init() override;
    void OnEnable() override {}
    void Update() override;
    void Render() override;
    void OnDisable() override {}
    void Release() override {}
    CPlayer* GetPlayer() const { return player; }

private:
    // 배경 타일 이미지
    CImage* tileImg;

    float   tileWorldSize;     // 타일 하나의 월드 크기(정사각형 한 변 길이)
    float   gridStep;          // 그리드 간격(타일 사이 간격 포함)
    float   density;           // 그리드 칸당 타일 배치 확률(0.0~1.0), 낮을수록 드문드문
    COLORREF colorKey;         // 투명색 키(타일 렌더용)

    // 랜덤성 재현을 위한 시드(해시)
    unsigned int noiseSeed;

private:
    // 화면에 보이는 월드 영역에 대해 그리드 좌표를 순회하며 확률적으로 타일을 그립니다.
    void RenderWorldTiles();
    // 결정적 해시 기반 의사난수 (좌표와 시드로부터 0~1 반환)
    float Hash01(int gx, int gy) const;
    // 플레이어 카메라 오프셋을 이용한 월드→스크린 변환
    Vec2 WorldToScreen(const Vec2& w) const;
    CPlayer* player = nullptr;
};