#pragma once

class CGameObject;
class CMissile;
class CMonster;
class CPlayer;

struct CombatStats
{
    float hp            = 100.f;
    float maxHp         = 100.f;
    float attack        = 10.f;
    float defense       = 0.f;      // 단순 감소치
    float critChance    = 0.1f;     // 0~1
    float critMultiplier= 1.5f;     // 치명타 배수
	float speed         = 200.f;    // 이동 속도
    bool  alive() const { return hp > 0.f; }
};

class CCombatSystem : public SingleTon<CCombatSystem>
{
    friend class SingleTon<CCombatSystem>;
private:
    CCombatSystem() {}
    ~CCombatSystem() {}

public:
    // 기본 데미지 공식: (공격력 - 방어력) * (크리티컬이면 배수)
    float CalculateDamage(const CombatStats& attacker, const CombatStats& victim, bool crit);

    // 실제 피해 적용 (사망 처리 등) + 선택적 결과 반환
    void ApplyDamage(CGameObject* attackerObj, CGameObject* victimObj,
                     CombatStats& attackerStats, CombatStats& victimStats,
                     float* damageOut = nullptr, bool* critOut = nullptr);

    bool IsCritical(const CombatStats& attacker);
    void HandleDeath(CGameObject* obj, CombatStats& stats);
    void DebugDamageLog(CGameObject* attackerObj, CGameObject* victimObj,
                        float dmg, bool crit);
};

#define COMBAT CCombatSystem::GetInstance()