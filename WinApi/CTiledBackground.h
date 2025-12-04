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

private:
    CPlayer* player = nullptr;
};