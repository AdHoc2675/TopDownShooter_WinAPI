#pragma once
#include "CGameObject.h"

struct BgElem
{
    enum Type { Rock, Bush, Pebble } type;
    Vec2 worldPos;
    float size;
    COLORREF color;
};

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

private:
    CImage* tileImg = nullptr;
    std::vector<BgElem> elems; // 고정 월드 좌표 요소
};