#include "pch.h"
#include "CCombatSystem.h"
#include "CGameObject.h"
#include "CEventManager.h"

float CCombatSystem::CalculateDamage(const CombatStats& attacker, const CombatStats& victim, bool crit)
{
    float base = attacker.attack - victim.defense;
    if (base < 1.f) base = 1.f;
    if (crit)
        base *= attacker.critMultiplier;
    return base;
}

bool CCombatSystem::IsCritical(const CombatStats& attacker)
{
    // 간단한 확률(추후 RNG 개선 가능)
    float r = (float)rand() / (float)RAND_MAX;
    return r < attacker.critChance;
}

void CCombatSystem::ApplyDamage(CGameObject* attackerObj, CGameObject* victimObj,
                                CombatStats& attackerStats, CombatStats& victimStats,
                                float* damageOut, bool* critOut)
{
    if (!attackerObj || !victimObj) return;
    if (!victimStats.alive()) return;

    // 크리티컬 1회 판정
    bool crit = IsCritical(attackerStats);

    float dmg = CalculateDamage(attackerStats, victimStats, crit);

    victimStats.hp -= dmg;
    if (victimStats.hp < 0.f) victimStats.hp = 0.f;

    DebugDamageLog(attackerObj, victimObj, dmg, crit);

    if (damageOut) *damageOut = dmg;
    if (critOut)   *critOut   = crit;

    if (!victimStats.alive())
        HandleDeath(victimObj, victimStats);
}

void CCombatSystem::HandleDeath(CGameObject* obj, CombatStats& stats)
{
    // 사망 처리(씬에서 삭제)
    EVENT->Delete(obj->GetScene(), obj);
}

void CCombatSystem::DebugDamageLog(CGameObject* attackerObj, CGameObject* victimObj,
                                   float dmg, bool crit)
{
    wstring msg = attackerObj->GetName() + TEXT(" -> ") +
                  victimObj->GetName() + TEXT(" : ") +
                  to_wstring((int)dmg) + (crit ? TEXT(" (CRIT)") : TEXT(""));
    Logger::Debug(msg);
}