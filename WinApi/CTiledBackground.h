#pragma once
#include "CGameObject.h"
#include "CPlayer.h"

struct BgElem
{
    enum Type { Rock, Bush, Pebble, GrassPatch, Mushroom} type;
    Vec2   worldPos;
    float  size;
    COLORREF color;
    int    variant; // 같은 타입 내 변주 선택
};

class CTiledBackground : public CGameObject
{
public:
    CTiledBackground();
    ~CTiledBackground() override {}
    void SetPlayer(CPlayer* player) { this->player = player; }

private:
    void Init() override;
    void OnEnable() override {}
    void Update() override;
    void Render() override;
    void OnDisable() override {}
    void Release() override {}

private:
    CImage* tileImg = nullptr;
    CPlayer* player = nullptr;
    std::vector<BgElem> elems; // 고정 월드 좌표 요소
};