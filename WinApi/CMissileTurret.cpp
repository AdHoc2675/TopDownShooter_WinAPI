#include "pch.h"
#include "CMissileTurret.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CMissile.h"
#include "CSceneStage01.h"

CMissileTurret::CMissileTurret()
{
    name           = TEXT("미사일 터렛");
    scale          = Vec2(40, 40);
    ownerPlayer    = nullptr;

    // 기본 전투 수치
    stats.attack         = 10.f;
    stats.defense        = 0.f;
    stats.critChance     = 0.0f;
    stats.critMultiplier = 1.5f;
    stats.hp             = 1.f;
    stats.maxHp          = 1.f;

    // 이동/발사 파라미터
    followDistance = 80.f;
    moveSpeed      = 220.f;
    fireInterval   = 1.0f;
    fireCooldown   = 0.f;

    missileSpeed   = 450.f;
    missileLife    = 1.0f;
}

CMissileTurret::~CMissileTurret() {}

void CMissileTurret::Init()
{

}

void CMissileTurret::OnEnable() {}

void CMissileTurret::Update()
{
    // 플레이어 추적 (고정 거리 유지)
    if (ownerPlayer)
    {
        Vec2 targetPos = ownerPlayer->GetWorldPos();
        Vec2 to = targetPos - worldPos;
        float dist = to.Length();
        if (dist > followDistance)
        {
            Vec2 dir = to.Normalized();
            pos += dir * moveSpeed * DT;
        }
    }

    // 발사 쿨다운
    if (fireCooldown > 0.f) fireCooldown -= DT;

    // 타겟 찾고 발사
    if (fireCooldown <= 0.f)
    {
        const float maxRange = 600.f;
        CMonster* target = FindNearestMonster(maxRange);
        if (target)
        {
            Vec2 from = worldPos;
            Vec2 to   = target->GetWorldPos();
            Vec2 dir  = (to - from).Normalized();

            // 발사 위치(앞쪽 약간)
            Vec2 spawnPos = from + dir * (scale.y * 0.5f + 8.f);
            SpawnMissile(spawnPos, dir);

            fireCooldown = fireInterval;
        }
    }
}

void CMissileTurret::Render()
{
    // 심플 렌더(원형)
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
    RENDER->SetBrush(BrushType::Solid, RGB(200, 230, 255));
    RENDER->Ellipse(
        renderPos.x - scale.x * 0.5f,
        renderPos.y - scale.y * 0.5f,
        renderPos.x + scale.x * 0.5f,
        renderPos.y + scale.y * 0.5f);
}

void CMissileTurret::OnDisable() {}
void CMissileTurret::Release() {}

void CMissileTurret::SpawnMissile(const Vec2& spawnPos, const Vec2& dir)
{
    CMissile* m = new CMissile();
    m->SetPos(spawnPos);
    m->SetDir(dir);

    // 공격 관련 수치만 상속 (속도/수명은 소환수 고유 설정으로 덮음)
    m->GetCombatStats().attack        = stats.attack;
    m->GetCombatStats().defense        = stats.defense;
    m->GetCombatStats().critChance     = stats.critChance;
    m->GetCombatStats().critMultiplier = stats.critMultiplier;

    // 이동/수명 설정
    // 미사일이 CombatStats.speed를 쓰지 않도록 했다면 moveSpeed로 관리
    // 현재 구현이 stats.speed를 사용 중이라면 아래처럼 설정:
    m->GetCombatStats().speed = missileSpeed;
    // lifeTime 직접 설정 필요 시 CMissile에 setter 추가하거나 생성 후 필드 접근하도록 변경

    EVENT->AddGameObject(GetScene(), m);
}

CMonster* CMissileTurret::FindNearestMonster(float maxRange)
{
    CSceneStage01* stage = dynamic_cast<CSceneStage01*>(GetScene());
    if (!stage) return nullptr;
    return stage->GetNearestEnemy(worldPos, maxRange);
}