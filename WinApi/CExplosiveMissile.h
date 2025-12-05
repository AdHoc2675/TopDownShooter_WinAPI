#pragma once
#include "CMissile.h"

class CExplosiveMissile : public CMissile
{
public:
    void OnCollisionEnter(CCollider* other) override;
    void Explode();
    // 필요한 멤버 변수 선언 (예시)
    float explosionRadius;
    float explosionDamageRatio;
    CombatStats stats;
    Vec2 worldPos;
    bool friendly;
}; 
