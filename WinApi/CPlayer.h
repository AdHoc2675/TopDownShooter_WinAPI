#pragma once
#include "CGameObject.h"
#include "CAnimator.h"
#include "CSound.h"
#include <CCombatSystem.h>

// 전방 선언
class CWeapon;

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	CombatStats& GetCombatStats() { return stats; }
	void	AddExp(int amount);

	void	SetWeapon(CWeapon* w) { weapon = w; }
	CWeapon* GetWeapon() const { return weapon; }

private:
	void	Init()			override;
	void	OnEnable()		override;
	void	Update()		override;
	void	Render()		override;
	void	OnDisable()		override;
	void	Release()		override;

private:
	void	AnimatorUpdate();
	void	OnCollisionEnter(CCollider* other) override;
	void	OnCollisionStay(CCollider* other) override;

	CAnimator*	animator;
	CombatStats stats;
	CImage*		heartFullImage;
	CImage*		heartEmptyImage;
	int 		level;
	int			exp;
	int			maxExp;
	float		hitCooldown;

	Vec2		moveDir;
	Vec2		lookDir;
	bool		isMove;

	CSound*		footstepSounds[3] = { nullptr, nullptr, nullptr };
	float       footstepInterval;
	float       footstepTimer;

	CWeapon*	weapon = nullptr;
};

