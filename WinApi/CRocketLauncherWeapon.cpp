#include "pch.h"
#include "CRocketLauncherWeapon.h"
#include "CExplosiveMissile.h"

CRocketLauncherWeapon::CRocketLauncherWeapon()
{
    name = TEXT("로켓 런처");
    scale = Vec2(18.f, 38.f);
    
    // 로켓 런처 스탯
    damage = 40.f;              // 높은 직격 피해
    fireCooldown = 1.2f;        // 느린 연사
    projectileCount = 1;        // 한 발씩
    maxChamberSize = 3.f;       // 작은 탄창
    curChamberSize = maxChamberSize;
    reloadTime = 2.5f;          // 긴 재장전
    pierceCount = 0;            // 관통 없음 (폭발로 보상)
    spreadAngleDeg = 0.f;       // 정확함
    missileSpeedMultiplier = 0.6f; // 느린 탄속
    
    explosionRadius = 120.f;
    explosionDamageRatio = 0.9f;
    
    curCooldown = 0.f;
    curReloadTime = 0.f;
}

void CRocketLauncherWeapon::CreateMissile(const Vec2& spawnPos, const Vec2& dir)
{
    CExplosiveMissile* missile = new CExplosiveMissile();
    missile->SetPos(spawnPos);
    missile->SetDir(dir);
    missile->SetExplosionRadius(explosionRadius);
    missile->SetExplosionDamageRatio(explosionDamageRatio);
    missile->SetFriendly(true);

    if (player)
    {
        missile->InheritCombat(player->GetCombatStats());
        CombatStats& mstats = missile->GetCombatStats();
        mstats.attack = damage;
    }

    missile->SetPierceCount(0);
    missile->SetMoveSpeed(350.f * missileSpeedMultiplier);
    missile->SetLifeTime(3.0f);

    EVENT->AddGameObject(GetScene(), missile);
}