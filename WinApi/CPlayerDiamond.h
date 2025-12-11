#pragma once
#include "CPlayer.h"

class CPlayerDiamond : public CPlayer
{
public:
    CPlayerDiamond();
    virtual ~CPlayerDiamond();

protected:
    void Init() override;
    void OnEnable() override;
    void Update() override;
    void Release() override;

    float dashCooldown = 0.f;
};