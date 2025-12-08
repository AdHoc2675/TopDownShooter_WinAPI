#pragma once
#include "CCombatSystem.h"
#include "CStatusEffect.h"
#include <vector>

class CPlayer;
class CCollider;
class CAnimator;
class CImage;
class CMonsterPoolManager;  // 전방 선언

class CMonster : public CGameObject
{
public:
    CMonster();
    virtual ~CMonster();
    void SetPlayer(CPlayer* p) { player = p; }
    CPlayer* GetPlayer() const { return player; }
    CombatStats& GetCombatStats() { return st; }
    CAnimator* GetAnimator() const { return animator; }

    // ===== 오브젝트 풀링 관련 =====
    virtual void Reset();
    void ReturnToPool();
    bool IsPooled() const { return fromPool; }
    void SetPooled(bool value) { fromPool = value; }

    // ===== 상태이상 관련 =====
    void ApplyStatusEffect(StatusEffectType type, int stacks = 1, float duration = 5.0f);
    void RemoveStatusEffect(StatusEffectType type);
    bool HasStatusEffect(StatusEffectType type) const;
    int GetStatusEffectStacks(StatusEffectType type) const;
    void ClearAllStatusEffects();

protected:
    CCollider* collider = nullptr;
    CAnimator* animator = nullptr;
    int ExpValue;
    int ExpCount;

    void DropExpOrb();
    void DropExpOrb(int value, int count);
    bool droppedExpOrb = false;

    void OnCollisionEnter(CCollider* other) override;
    void OnCollisionStay(CCollider* other)  override;
    void OnCollisionExit(CCollider* other)  override;

    // 상태이상 업데이트/렌더링
    void UpdateStatusEffects();
    void RenderStatusEffects();

    // 상태이상 목록
    std::vector<StatusEffect> statusEffects;

    // 풀링 관련 멤버 변수
    bool fromPool = false;

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;

private:
    CPlayer*    player = nullptr;
    CombatStats st;
};

