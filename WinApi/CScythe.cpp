#include "pch.h"
#include "CScythe.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CCombatSystem.h"
#include "CCollider.h"

CScythe::CScythe()
{
    name        = TEXT("낫 소환수");
    scale       = Vec2(48, 48);

    // 기본 전투 수치
    stats.attack         = 12.f;
    stats.defense        = 0.f;
    stats.critChance     = 0.0f;
    stats.critMultiplier = 1.0f;
    stats.hp             = 1.f;
    stats.maxHp          = 1.f;

    // 공전 파라미터
    orbitRadius = 200.f;
    angularSpeed = 2.2f;   // 라디안/초
    orbitAngle = 0.f;

    // 자전 파라미터
    spinSpeed = 8.0f;   // 라디안/초
    spinAngle = 0.f;


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

    // 자전
    spinAngle += spinSpeed * DT;
    if (spinAngle > 6.2831853f) spinAngle -= 6.2831853f;
}

void CScythe::UpdateOrbit()
{
    orbitAngle += angularSpeed * DT;
    if (orbitAngle > 6.2831853f) orbitAngle -= 6.2831853f;

    Vec2 center = ownerPlayer->GetWorldPos();
    Vec2 offset(cosf(orbitAngle) * orbitRadius, sinf(orbitAngle) * orbitRadius);
    pos = center + offset;
}

void CScythe::Render()
{
    if (scytheImage)
    {
        // 중심, 크기, 라디안 회전, 컬러키 마젠타
        RENDER->RotateImage(
            scytheImage,
            renderPos.x, renderPos.y,
            scale.x, scale.y,
            spinAngle,
            RGB(255, 0, 255));
    }
    else
    {
        // 폴백
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
