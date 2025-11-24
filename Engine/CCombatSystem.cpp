#include "pch.h"
#include "CCombatSystem.h"
#include "CGameObject.h"
#include "CEventManager.h"

float CCombatSystem::CalculateDamage(const CombatStats& attacker, const CombatStats& victim, bool& critOut)
{
    float base = attacker.attack - victim.defense;
    if (base < 1.f) base = 1.f; // 최소 1
    bool crit = IsCritical(attacker);
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
                                CombatStats& attackerStats, CombatStats& victimStats)
{
    if (!attackerObj || !victimObj) return;
    if (!victimStats.alive()) return;

    bool crit = false;

    // 치명타 여부 계산을 위해 별도 호출
    float r = (float)rand() / (float)RAND_MAX;
    crit = (r < attackerStats.critChance);

    float base = CalculateDamage(attackerStats, victimStats, crit);;

    victimStats.hp -= base;
    if (victimStats.hp < 0.f) victimStats.hp = 0.f;

    DebugDamageLog(attackerObj, victimObj, base, crit);

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