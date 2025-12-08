#pragma once
#include "CCombatSystem.h"

class CMissile : public CGameObject
{
public:
	CMissile();
	virtual ~CMissile();

	void OnCollisionEnter(CCollider* other) override;
	// 공격 관련 수치만 상속 (이동속도 등은 무시)
	void InheritCombat(const CombatStats& other)
	{
		stats.attack         = other.attack;
		stats.defense        = other.defense;
		stats.critChance     = other.critChance;
		stats.critMultiplier = other.critMultiplier;
	}

	void SetDir(const Vec2 dir) { this->dir = dir.Normalized(); }
	void SetFriendly(bool friendly) { this->friendly = friendly; }
	void SetLifeTime(float time) { this->lifeTime = time; }
	void SetMoveSpeed(float speed) { this->moveSpeed = speed; }
	void SetPierceCount(int count) { this->pierceCount = count; }
	void SetAppliesBurn(bool apply, int stacks = 1, float duration = 5.0f, float chance = 1.0f)
	{
		appliesBurn = apply;
		burnStacks = stacks;
		burnDuration = duration;
		burnChance = chance;
	}

	CombatStats& GetCombatStats() { return stats; }
	bool GetFriendly() const { return friendly; }
	int GetPierceCount() const { return pierceCount; }
	bool GetAppliesBurn() const { return appliesBurn; }
	int GetBurnStacks() const { return burnStacks; }
	float GetBurnDuration() const { return burnDuration; }
	float GetBurnChance() const { return burnChance; }

protected:
	bool appliesBurn = false;
	int burnStacks = 1;
	float burnDuration = 5.0f;
	float burnChance = 1.0f;

private:
	void Init() override;
	void OnEnable() override;
	void Update() override;
	void Render() override;
	void OnDisable() override;
	void Release() override;


private:
	Vec2        dir;
	float       lifeTime;
	float       moveSpeed;
	CombatStats stats;		// 공격 수치
	bool        friendly;	// 아군 여부
	int			pierceCount;// 관통 횟수
};
