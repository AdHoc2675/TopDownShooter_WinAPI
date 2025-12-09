#pragma once
#include "CMonster.h"
#include "CMissile.h"

class CRangedMonster : public CMonster
{
public:
    CRangedMonster();
    virtual ~CRangedMonster();

    //// 풀링용 Reset 오버라이드
    //void Reset() override;

private:
    void Init() override;
    void Update() override;
    void Render() override;

    void TryFire();
    void SpawnMissile(const Vec2& dir);

private:
    float fireInterval;
    float fireCooldown;
    float missileSpeed;
    float attackRange;
};