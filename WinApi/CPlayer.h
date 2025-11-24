#pragma once
class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

private:
	void	Init()			override;
	void	OnEnable()		override;
	void	Update()		override;
	void	Render()		override;
	void	OnDisable()		override;
	void	Release()		override;

private:
	void	AnimatorUpdate();

	CAnimator*	animator;
	float		speed;
	int 		life;
	int 		level;
	int			exp;
	int			maxExp;

	Vec2		moveDir;
	Vec2		lookDir;
	bool		isMove;
};

