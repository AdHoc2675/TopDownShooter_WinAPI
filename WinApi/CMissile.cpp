#include "pch.h"
#include "CMissile.h"

#include "CGame.h"

CMissile::CMissile()
{
	name = TEXT("미사일");
	scale = Vec2(15, 15);
	dir = Vec2(0, -1);
	speed = 700.f;
	lifeTime = 3.f;
}

CMissile::~CMissile()
{
}

void CMissile::Init()
{
	CCollider* collider = new CCollider();
	collider->SetScale(Vec2(20, 20));
	collider->SetLayer(Layer::Missile);
	AddChild(collider);
}

void CMissile::OnEnable()
{
}

void CMissile::Update()
{
	pos += dir * speed * DT;

	// 개선점: 미사일이 일정 시간 경과 시 삭제
	lifeTime = lifeTime - DT;
	if (lifeTime <= 0.f)
		EVENT->Delete(GetScene(), this);

}

void CMissile::Render()
{
	RENDER->Ellipse(
		renderPos.x - scale.x * 0.5f,
		renderPos.y - scale.y * 0.5f,
		renderPos.x + scale.x * 0.5f,
		renderPos.y + scale.y * 0.5f);
}

void CMissile::OnDisable()
{
}

void CMissile::Release()
{
}

void CMissile::OnCollisionEnter(CCollider* other)
{
	EVENT->Delete(GetScene(), this);
}
