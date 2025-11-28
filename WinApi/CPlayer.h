#pragma once
#include "CCombatSystem.h"

struct CombatStats;
class CAnimator;
class CImage;

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();
	CombatStats& GetCombatStats() { return stats; }
	void	AddExp(int amount);

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

	// 발소리 관련 (추가)
	CSound*		footstepSounds[3] = { nullptr, nullptr, nullptr };
	float       footstepInterval; // 발소리 간격(초)
	float       footstepTimer;   // 다음 발소리까지 남은 시간

};

