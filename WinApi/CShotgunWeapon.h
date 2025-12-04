#pragma once
#include "CWeapon.h"

class CShotgunWeapon : public CWeapon
{
public:
    CShotgunWeapon();
    virtual ~CShotgunWeapon() {}

    // 필요 시 샷건 특화 설정(예: 산탄 각도 등) 조정 가능
};