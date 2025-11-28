#pragma once
#include "CMonster.h"
#include "CMissile.h"

class CRangedMonster : public CMonster
{
public:
    CRangedMonster();
    virtual ~CRangedMonster();

private:
    void Init() override;
    void Update() override;

    void TryFire();
    void SpawnMissile(const Vec2& dir);

private:
    float fireInterval;
    float fireCooldown;
    float missileSpeed;
    float attackRange;
};