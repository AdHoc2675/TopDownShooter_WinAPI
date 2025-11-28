#pragma once
#include "CMonster.h"
#include "CMissile.h"

class CEliteWingedMonster : public CMonster
{
public:
	CEliteWingedMonster();
	virtual ~CEliteWingedMonster();

private:
	void Init() override;
	void Update() override;

	void TryFire();
	void SpawnMissile(const Vec2& dir);
	void ChargeAttack();

private:
	float fireInterval;
	float fireCooldown;
	float missileSpeed;
	float attackRange;
};

