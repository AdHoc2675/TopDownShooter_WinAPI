#pragma once
#include "CGameObject.h"
#include <CCombatSystem.h>

class CExplosionEffect : public CGameObject
{
public:
    CExplosionEffect();
    virtual ~CExplosionEffect();
    
    // 폭발 설정
    void Configure(const Vec2& center, float radius, float damage, 
                   bool friendly = true, float lifetime = 0.3f);
    
    // 전투 스탯 접근 (피해 계산용)
    CombatStats& GetCombatStats() { return stats; }

private:
    void Init() override;
    void OnEnable() override;
    void Update() override;
    void Render() override;
    void OnDisable() override;
    void Release() override;
    
    void OnCollisionEnter(CCollider* other) override;
    
    float lifetime = 0.3f;
    float maxLifetime = 0.3f;
    float explosionRadius = 80.f;
    
    bool friendly = true;        // true: 플레이어편(몬스터 피해), false: 적편(플레이어 피해)
    bool hasDealtDamage = false; // 한 번만 피해 주도록
    
    CombatStats stats;           // 폭발 피해량 등
    CCollider* collider = nullptr;
};