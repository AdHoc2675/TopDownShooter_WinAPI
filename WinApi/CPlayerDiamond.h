#pragma once
#include "CPlayer.h"

class CPlayerDiamond : public CPlayer
{
public:
    CPlayerDiamond();
    virtual ~CPlayerDiamond();

private:
    void Init() override;        // 리소스/애니메이터 설정
    void OnEnable() override;
    void Update() override;      // 특수 동작 오버라이드(필요 시)
    void Release() override;

    // 캐릭터 고유 상태
    float dashCooldown = 0.f;
};