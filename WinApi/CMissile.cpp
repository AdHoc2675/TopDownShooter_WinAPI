#include "pch.h"
#include "CMissile.h"
#include "CCombatSystem.h"
#include "CPlayer.h"

CMissile::CMissile()
{
    name      = TEXT("미사일");
    scale     = Vec2(15, 15);
    dir       = Vec2(0, -1);
    lifeTime  = 0.75f;
    moveSpeed = 500.f;
    friendly  = true;
	pierceCount = 0;

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
    collider->SetScale(scale);
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
    RENDER->SetBrush(BrushType::Solid, friendly ? RGB(255, 255, 255) : RGB(255, 180, 140));
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
    // 아군 미사일: 몬스터에서 처리
    if (friendly)
    {
        if (other->GetLayer() == Layer::Monster) {
            if (pierceCount > 0)
            {
                pierceCount--;
                return;
            }
            else {
                EVENT->Delete(GetScene(), this);
            }
            return;
        }
    }
    // 적 미사일: 플레이어 충돌 시 피해 적용
    else if (!friendly && other->GetLayer() == Layer::Player)
    {
        CGameObject* playerObj = other->GetOwner();
        CPlayer* player = dynamic_cast<CPlayer*>(playerObj);
        if (player)
        {
            // 플레이어 hitCooldown 체크 추가
            if (player->IsHitCooldown())
            {
                // 쿨다운 중이면 피해 없이 미사일만 제거
                EVENT->Delete(GetScene(), this);
                return;
            }
            
            CombatStats& victimStats = player->GetCombatStats();
            float dealt = 0.f;
            bool crit = false;
            COMBAT->ApplyDamage(this, player, stats, victimStats, &dealt, &crit);
            
            // 피격 후 쿨다운 설정
            player->SetHitCooldown(1.0f);
            
            // 피격 사운드 재생
            player->PlayHitSound();
        }
        EVENT->Delete(GetScene(), this);
    }
}
