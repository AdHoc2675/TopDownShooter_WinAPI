#pragma once
#include "CCombatSystem.h"

class CMissile : public CGameObject
{
public:
	CMissile();
	virtual ~CMissile();

	void SetDir(const Vec2 dir) { this->dir = dir.Normalized(); }
	void SetFriendly(bool friendly) { this->friendly = friendly; }
	void SetLifeTime(float time) { this->lifeTime = time; }
	void SetMoveSpeed(float speed) { this->moveSpeed = speed; }

	// 공격 관련 수치만 상속 (이동속도 등은 무시)
	void InheritCombat(const CombatStats& other)
	{
		stats.attack         = other.attack;
		stats.defense        = other.defense;
		stats.critChance     = other.critChance;
		stats.critMultiplier = other.critMultiplier;
	}

	CombatStats& GetCombatStats() { return stats; }
	bool GetFriendly() const { return friendly; }

private:
	void Init() override;
	void OnEnable() override;
	void Update() override;
	void Render() override;
	void OnDisable() override;
	void Release() override;

	void OnCollisionEnter(CCollider* other) override;

private:
	Vec2        dir;
	float       lifeTime;
	float       moveSpeed;
	CombatStats stats;
	bool        friendly;
};
