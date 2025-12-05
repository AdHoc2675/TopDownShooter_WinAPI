#pragma once
#include "pch.h"
#include "CExplosiveMissile.h"
#include "CExplosionEffect.h"

CExplosiveMissile::CExplosiveMissile()
{
}

void CExplosiveMissile::OnCollisionEnter(CCollider* other)
{
    if (friendly && other->GetLayer() == Layer::Monster)
    {
        Explode();
        EVENT->Delete(GetScene(), this);
        return;
    }
    
    // 적 미사일이 플레이어와 충돌
    if (!friendly && other->GetLayer() == Layer::Player)
    {
        Explode();
        EVENT->Delete(GetScene(), this);
        return;
    }   
}

void CExplosiveMissile::Explode()
{
    
    // 폭발 이펙트 생성 (콜라이더가 자동으로 피해 처리)
    CExplosionEffect* effect = new CExplosionEffect();
    effect->Configure(
        worldPos,                      // 폭발 중심
        explosionRadius,               // 반경
        stats.attack * explosionDamageRatio, // 피해량
        true,                          // 아군 폭발dd
        0.4f                           // 지속 시간
    );
    
    // 폭발에 원본 스탯 상속 (크리티컬 등)
    CombatStats& expStats = effect->GetCombatStats();
    expStats.critChance = stats.critChance;
    expStats.critMultiplier = stats.critMultiplier;
    
    EVENT->AddGameObject(GetScene(), effect);
}