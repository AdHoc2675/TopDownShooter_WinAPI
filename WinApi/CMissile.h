#pragma once
#include "CCombatSystem.h"

class CMissile : public CGameObject
{
public:
	CMissile();
	virtual ~CMissile();

	void SetDir(const Vec2 dir) { this->dir = dir.Normalized(); }

	// 공격 관련 수치만 상속 (이동속도 등은 무시)
	void InheritCombat(const CombatStats& other)
	{
		stats.attack         = other.attack;
		stats.defense        = other.defense;
		stats.critChance     = other.critChance;
		stats.critMultiplier = other.critMultiplier;
		// hp / maxHp 는 투사체 독립 운영 가능 (필요시 조정)
	}

	CombatStats& GetCombatStats() { return stats; }

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
};
