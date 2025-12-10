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

	// 피해 쿨다운 관련 함수 추가
	bool	IsHitCooldown() const { return hitCooldown > 0.f; }
	void	SetHitCooldown(float time) { hitCooldown = time; }
	float	GetHitCooldown() const { return hitCooldown; }

	// 피격 사운드 재생
	void	PlayHitSound();

public:
	int GetLevel() const { return level; }

protected:
	void	Init()			override;
	void	OnEnable()		override;
	void	Update()		override;
	void	Render()		override;
	void	OnDisable()		override;
	void	Release()		override;

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
	CSound*		hitSound = nullptr;
	CSound*		addExpSound = nullptr;

	CWeapon*	weapon = nullptr;
};

