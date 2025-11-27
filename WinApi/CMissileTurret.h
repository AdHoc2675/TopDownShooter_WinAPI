#pragma once
#include "CCombatSystem.h"

class CMonster;
class CPlayer;
class CMissile;

class CMissileTurret : public CGameObject
{
public:
    CMissileTurret();
    virtual ~CMissileTurret();

    void SetOwnerPlayer(CPlayer* p) { ownerPlayer = p; }
	CombatStats& GetCombatStats() { return stats; }

private:
    void Init() override;
    void OnEnable() override;
    void Update() override;
    void Render() override;
    void OnDisable() override;
    void Release() override;

    void SpawnMissile(const Vec2& spawnPos, const Vec2& dir);
    CMonster* FindNearestMonster(float maxRange);

private:
    CPlayer*    ownerPlayer;
    CombatStats stats;          // 공격 관련 수치만 사용
    float       followDistance; // 플레이어로부터 거리 유지
    float       moveSpeed;      // 소환수 이동속도
    float       fireInterval;   // 발사 간격(초)
    float       fireCooldown;   // 남은 쿨타임
    float       missileSpeed;   // 투사체 이동속도
    float       missileLife;    // 투사체 수명
};