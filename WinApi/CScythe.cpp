#include "pch.h"
#include "CScythe.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CSceneStage01.h"
#include "CCollider.h"
#include "CCombatSystem.h"

CScythe::CScythe()
{
    name        = TEXT("낫 소환수");
    scale       = Vec2(40, 40);
    // 기본 전투 수치
    stats.attack         = 12.f;
    stats.defense        = 0.f;
    stats.critChance     = 0.0f;
    stats.critMultiplier = 1.0f;
    stats.hp             = 1.f;
    stats.maxHp          = 1.f;

    orbitRadius = 200.f;
    angularSpeed = 2.0f;    // 라디안/초
    orbitAngle = 0.f;
}

CScythe::~CScythe() {}

void CScythe::Init()
{
    collider = new CCollider();
    collider->SetScale(Vec2(45, 45));
    collider->SetLayer(Layer::Missile);
    AddChild(collider);

    scytheImage = LOADIMAGE(TEXT("Scythe"), TEXT("Image\\T_Scythe.bmp"));
}

void CScythe::OnEnable() {}

void CScythe::Update()
{
    if (ownerPlayer)
        UpdateOrbit();
}

void CScythe::UpdateOrbit()
{
    // 각도 증가(라디안/초)
    orbitAngle += angularSpeed * DT;
    if (orbitAngle > 6.28318f) orbitAngle -= 6.28318f;

    // 플레이어 중심 기준 위치 계산
    Vec2 center = ownerPlayer->GetWorldPos();
    Vec2 offset(cosf(orbitAngle) * orbitRadius, sinf(orbitAngle) * orbitRadius);
    pos = center + offset;
}

void CScythe::Render()
{
    if (scytheImage)
    {
        RENDER->TransparentImage(
            scytheImage,
            renderPos.x - scale.x * 0.5f,
            renderPos.y - scale.y * 0.5f,
            renderPos.x + scale.x * 0.5f,
            renderPos.y + scale.y * 0.5f,
            RGB(255, 0, 255));
    }
    else
    {
        RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
        RENDER->SetBrush(BrushType::Solid, RGB(200, 255, 200));
        RENDER->Ellipse(
            renderPos.x - scale.x * 0.5f,
            renderPos.y - scale.y * 0.5f,
            renderPos.x + scale.x * 0.5f,
            renderPos.y + scale.y * 0.5f);
    }
}

void CScythe::OnDisable() {}
void CScythe::Release() {}

void CScythe::OnCollisionEnter(CCollider* other)
{

}
