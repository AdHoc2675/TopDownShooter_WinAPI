#include "pch.h"
#include "CMissile.h"
#include "CCombatSystem.h"

CMissile::CMissile()
{
    name      = TEXT("미사일");
    scale     = Vec2(15, 15);
    dir       = Vec2(0, -1);
    lifeTime  = 0.75f;
    moveSpeed = 500.f;

    stats.attack         = 10.f;
    stats.hp             = 1.f;
    stats.maxHp          = 1.f;
    stats.defense        = 0.f;
    stats.critChance     = 0.0f;
    stats.critMultiplier = 1.5f;
}

CMissile::~CMissile() {}

void CMissile::Init()
{
    CCollider* collider = new CCollider();
    collider->SetScale(Vec2(20, 20));
    collider->SetLayer(Layer::Missile);
    AddChild(collider);
}

void CMissile::OnEnable() {}

void CMissile::Update()
{
    pos += dir * moveSpeed * DT;

    lifeTime -= DT;
    if (lifeTime <= 0.f)
        EVENT->Delete(GetScene(), this);
}

void CMissile::Render()
{
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
    RENDER->SetBrush(BrushType::Solid, RGB(255, 255, 255));
    RENDER->Ellipse(
        renderPos.x - scale.x * 0.5f,
        renderPos.y - scale.y * 0.5f,
        renderPos.x + scale.x * 0.5f,
        renderPos.y + scale.y * 0.5f);
}

void CMissile::OnDisable() {}

void CMissile::Release() {}

void CMissile::OnCollisionEnter(CCollider* other)
{
    if (other->GetLayer() == Layer::Monster)
    {
        // 데미지 적용은 몬스터 쪽에서 처리하도록 설계 가능
    }
    EVENT->Delete(GetScene(), this);
}
