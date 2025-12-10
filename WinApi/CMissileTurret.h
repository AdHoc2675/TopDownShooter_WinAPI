#pragma once
#include "CCombatSystem.h"
#include <vector>

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
    CPlayer* GetOwnerPlayer() const { return ownerPlayer; }

    // 모든 활성 터렛 접근 (CScythe 패턴과 동일)
    static const std::vector<CMissileTurret*>& GetAll() { return s_instances; }

    // 업그레이드 적용용 인터페이스
    void AddPierce(int n) { missilePierce += n; }
    void AddBurn(int stacks) { missileAppliesBurn = true; missileBurnStacks += stacks; }
    void EnablePierceDamageBonus() { missilePierceDamageBonusEnabled = true; }

    // getters for SpawnMissile
    int GetMissilePierce() const { return missilePierce; }
    bool GetMissileAppliesBurn() const { return missileAppliesBurn; }
    int GetMissileBurnStacks() const { return missileBurnStacks; }
    float GetMissileBurnDuration() const { return missileBurnDuration; }
    float GetMissileBurnChance() const { return missileBurnChance; }
    bool IsPierceDamageBonusEnabled() const { return missilePierceDamageBonusEnabled; }

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
    CPlayer*    ownerPlayer = nullptr;
    CombatStats stats;          // 공격 관련 수치만 사용
    float       followDistance = 80.f; // 플레이어로부터 거리 유지
    float       moveSpeed = 220.f;      // 소환수 이동속도
    float       fireInterval = 1.0f;   // 발사 간격(초)
    float       fireCooldown = 0.f;   // 남은 쿨타임
    float       missileSpeed = 450.f;   // 투사체 이동속도
    float       missileLife = 2.0f;    // 투사체 수명

    CAnimator* animator = nullptr;

    bool  isFiring = false;
    float fireAnimTime = 0.f;

    // 소환수 발사체 속성 (업그레이드로 변경 가능)
    int   missilePierce = 3;           // 기본 관통
    bool  missileAppliesBurn = false;
    int   missileBurnStacks = 0;
    float missileBurnDuration = 5.0f;
    float missileBurnChance = 1.0f;
    bool  missilePierceDamageBonusEnabled = false; // SummonRangedAtkUp 적용 여부

    // 정적 인스턴스 목록
    static std::vector<CMissileTurret*> s_instances;
};