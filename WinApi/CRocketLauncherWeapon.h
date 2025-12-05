#pragma once
#include "CWeapon.h"

class CRocketLauncherWeapon : public CWeapon
{
public:
    CRocketLauncherWeapon();
    virtual ~CRocketLauncherWeapon() {}

protected:
    void CreateMissile(const Vec2& spawnPos, const Vec2& dir);

private:
    float explosionRadius = 120.f;
    float explosionDamageRatio = 0.9f;
};