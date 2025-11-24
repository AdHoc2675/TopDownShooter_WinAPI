#include "pch.h"
#include "CMissile.h"
#include "CCombatSystem.h"

CMissile::CMissile()
{
	name = TEXT("미사일");
	scale = Vec2(15, 15);
	dir = Vec2(0, -1);
	speed = 700.f;
	lifeTime = 3.f;

	stats.attack = 10.f;
	stats.hp = 1.f;
	stats.maxHp = 1.f;
	stats.defense = 0.f;
	stats.critChance = 0.0f;
	stats.critMultiplier = 1.5f;
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
	pos = pos + dir * speed * DT;

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
	if (other->GetLayer() == Layer::Monster)
	{
		CGameObject* monsterObj = other->GetOwner();
		if (monsterObj)
		{
			// 몬스터가 ApplyDamage 호출하는 방식으로 할 수도 있으나 여기서는 시스템 직접 호출
			// (몬스터 쪽으로 이동시키고 싶으면 이 부분 생략)
		}
	}
	EVENT->Delete(GetScene(), this);
}
