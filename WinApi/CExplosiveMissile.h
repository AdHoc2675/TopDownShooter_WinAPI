#pragma once
#include "CMissile.h"

class CExplosiveMissile : public CMissile
{
public:
	CExplosiveMissile();
	~CExplosiveMissile() {}

    void OnCollisionEnter(CCollider* other) override;
    void Explode();

	void SetExplosionRadius(float radius) { explosionRadius = radius; }
	void SetExplosionDamageRatio(float ratio) { explosionDamageRatio = ratio; }

	float GetExplosionRadius() const { return explosionRadius; }
	float GetExplosionDamageRatio() const { return explosionDamageRatio; }

private:
    float explosionRadius;
    float explosionDamageRatio;
    CombatStats stats;
    Vec2 worldPos;
    bool friendly;
}; 
