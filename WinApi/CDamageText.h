#pragma once
#include "pch.h"

class CDamageText : public CGameObject
{
public:
    CDamageText() = default;
    virtual ~CDamageText() = default;

    void Configure(const Vec2& worldPos, int amount, bool crit, float life = 0.6f, COLORREF color = RGB(255,0,0))
    {
        pos = worldPos;
        text = L"-" + to_wstring(amount);
        if (crit) { text = L"CRIT " + text; textColor = RGB(255, 100, 0); textSize = 18; }
        else      { textColor = color; }
        lifetime = life;
        vel = Vec2(0.f, -60.f); // 위로 살짝 떠오르기
    }

private:
    void Init() override {}
    void OnEnable() override {}
    void Update() override
    {
        pos += vel * DT;
        lifetime -= DT;
        if (lifetime <= 0.f) EVENT->Delete(GetScene(), this);
    }
    void Render() override
    {
        RENDER->SetText(textSize, textColor, TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(renderPos.x, renderPos.y, text);
    }
    void OnDisable() override {}
    void Release() override {}

private:
    wstring  text;
    float    lifetime = 0.6f;
    Vec2     vel;
    COLORREF textColor = RGB(255,0,0);
	int      textSize = 14;
};