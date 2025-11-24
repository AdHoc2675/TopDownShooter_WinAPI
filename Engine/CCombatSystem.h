#pragma once

// 전방 선언
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
    bool  alive() const { return hp > 0.f; }
};

class CCombatSystem : public SingleTon<CCombatSystem>
{
    friend class SingleTon<CCombatSystem>;
private:
    CCombatSystem() {}
    ~CCombatSystem() {}

public:
    // 기본 데미지 공식: (공격력 - 방어력) * 치명타
    float CalculateDamage(const CombatStats& attacker, const CombatStats& victim);

    // 실제 피해 적용 (사망 처리 등)
    void ApplyDamage(CGameObject* attackerObj, CGameObject* victimObj,
                     CombatStats& attackerStats, CombatStats& victimStats);

    // 치명타 판정
    bool IsCritical(const CombatStats& attacker);

    // HP 감소 후 사망 처리 콜백 분리 가능
    void HandleDeath(CGameObject* obj, CombatStats& stats);

    // 로깅 / 디버그 출력(옵션)
    void DebugDamageLog(CGameObject* attackerObj, CGameObject* victimObj,
                        float dmg, bool crit);
};

#define COMBAT CCombatSystem::GetInstance()