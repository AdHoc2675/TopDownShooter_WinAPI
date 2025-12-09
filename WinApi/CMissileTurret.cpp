#include "pch.h"
#include "CMissileTurret.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CMissile.h"
#include "CSceneStage01.h"
#include <algorithm>

// 정적 멤버 정의
std::vector<CMissileTurret*> CMissileTurret::s_instances;

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

    isFiring       = false;
    fireAnimTime   = 0.f;

    // 기본 소환수 발사체 세팅
    missilePierce = 3;
    missileAppliesBurn = false;
    missileBurnStacks = 0;
    missileBurnDuration = 5.0f;
    missileBurnChance = 1.0f;
    missilePierceDamageBonusEnabled = false;

    // 인스턴스 목록에 등록
    s_instances.push_back(this);
}

CMissileTurret::~CMissileTurret()
{
    // 인스턴스 목록에서 제거
    auto it = std::find(s_instances.begin(), s_instances.end(), this);
    if (it != s_instances.end())
        s_instances.erase(it);
}

void CMissileTurret::Init()
{
    animator = new CAnimator();

    CImage* moveRight = LOADIMAGE(TEXT("T_GhostPet0"), TEXT("Image\\T_GhostPet0.bmp"));
    animator->CreateAnimation(TEXT("MoveRight"), moveRight,
        0.15f, 6, true,
        Vec2(0.f, 0.f),        // 첫 프레임 시작 위치
        Vec2(16.f, 16.f),      // 프레임 크기
        Vec2(16.f, 0.f));      // 프레임 간 이동(가로)

    // 좌측 이동: T_GhostMonster1 (64x64 / 7프레임)
    CImage* moveLeft = LOADIMAGE(TEXT("T_GhostPet1"), TEXT("Image\\T_GhostPet1.bmp"));
    animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
        0.15f, 6, true,
        Vec2(0.f, 0.f),        // 첫 프레임 시작 위치
        Vec2(16.f, 16.f),      // 프레임 크기
        Vec2(16.f, 0.f));      // 프레임 간 이동(가로)

    CImage* fireRight = LOADIMAGE(TEXT("T_GhostPet0"), TEXT("Image\\T_GhostPet0.bmp"));
    animator->CreateAnimation(TEXT("FireRight"), fireRight,
        0.15f, 4, false,       // 반복 없이 한 번만 재생
        Vec2(0.f, 16.f),
        Vec2(16.f, 16.f),
        Vec2(16.f, 0.f));

    CImage* fireLeft = LOADIMAGE(TEXT("T_GhostPet1"), TEXT("Image\\T_GhostPet1.bmp"));
    animator->CreateAnimation(TEXT("FireLeft"), fireLeft,
        0.15f, 4, false,       // 반복 없이 한 번만 재생
        Vec2(32.f, 16.f),
        Vec2(16.f, 16.f),
        Vec2(16.f, 0.f));

    AddChild(animator);
    animator->Play(TEXT("MoveRight"), true);
    animator->SetRatio(2.0f);
}

void CMissileTurret::OnEnable() {}

void CMissileTurret::Update()
{
    Vec2 targetPos = ownerPlayer->GetWorldPos();
    Vec2 toPlayer = targetPos - worldPos;
    float dist = toPlayer.Length();
    Vec2 playerDir = toPlayer.Normalized();

    // 플레이어 추적 (고정 거리 유지)
    if (ownerPlayer)
    {
        if (dist > followDistance)
        {
            pos += playerDir * moveSpeed * DT;
        }
    }

    // Fire 애니메이션 타이머 업데이트
    if (isFiring)
    {
        fireAnimTime -= DT;
        if (fireAnimTime <= 0.f)
        {
            isFiring = false;
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
            Vec2 fireDir = (to - from).Normalized();

            // 발사 위치(앞쪽 약간)
            Vec2 spawnPos = from + fireDir * (scale.y * 0.5f + 8.f);
            SpawnMissile(spawnPos, fireDir);

            fireCooldown = fireInterval;

            // Fire 애니메이션 시작
            isFiring = true;
            fireAnimTime = 0.15f * 4;  // 프레임당 시간 * 프레임 수

            if (fireDir.x < -0.01f)
                animator->Play(TEXT("FireLeft"), true);
            else
                animator->Play(TEXT("FireRight"), true);
        }
    }

    // Fire 애니메이션 중이 아닐 때만 Move 애니메이션 재생
    if (!isFiring)
    {
        if (playerDir.x < -0.01f)
            animator->Play(TEXT("MoveLeft"), false);
        else
            animator->Play(TEXT("MoveRight"), false);
    }
}

void CMissileTurret::Render()
{

}

void CMissileTurret::OnDisable() {}
void CMissileTurret::Release() {}

void CMissileTurret::SpawnMissile(const Vec2& spawnPos, const Vec2& dir)
{
    CMissile* m = new CMissile();
    m->SetPos(spawnPos);
    m->SetDir(dir);

    // 기본 공격력: 터렛의 stats.attack 에 더해 관통 기반 보너스를 적용할 수 있음
    float baseAttack = stats.attack;
    if (missilePierceDamageBonusEnabled)
    {
        baseAttack += static_cast<float>(missilePierce) * 3.f; // 관통 * 3 만큼 추가 피해
    }

    m->GetCombatStats().attack        = baseAttack;
    m->GetCombatStats().defense       = stats.defense;
    m->GetCombatStats().critChance    = stats.critChance;
    m->GetCombatStats().critMultiplier= stats.critMultiplier;
    m->GetCombatStats().speed         = missileSpeed;

    // 관통 횟수 설정
    m->SetPierceCount(missilePierce);

    // 화상 설정 전달
    if (missileAppliesBurn)
        m->SetAppliesBurn(true, missileBurnStacks, missileBurnDuration, missileBurnChance);

    EVENT->AddGameObject(GetScene(), m);
}

CMonster* CMissileTurret::FindNearestMonster(float maxRange)
{
    CSceneStage01* stage = dynamic_cast<CSceneStage01*>(GetScene());
    if (!stage) return nullptr;
    return stage->GetNearestEnemy(worldPos, maxRange);
}