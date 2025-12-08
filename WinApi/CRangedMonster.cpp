#include "pch.h"
#include "CRangedMonster.h"
#include "CPlayer.h"
#include "CMissile.h"
#include "CCombatSystem.h"

CRangedMonster::CRangedMonster()
{
    name  = TEXT("원거리 몬스터");
    scale = Vec2(40, 40);

    // 기본 전투 수치 조정 (GetCombatStats() 통해 설정)
    CombatStats& st = GetCombatStats();
    st.hp             = 80.f;
    st.maxHp          = 80.f;
    st.attack         = 1.f;
    st.defense        = 0.f;
    st.critChance     = 0.00f;
    st.critMultiplier = 1.0f;
    st.speed          = 70.f;   // 기본 몬스터보다 느림

    fireInterval  = 1.8f;
    fireCooldown  = 0.f;
    missileSpeed  = 100.f;       // 느린 투사체
    attackRange   = 650.f;       // 사거리

    ExpValue = 20;
    ExpCount = 1;

    
}

CRangedMonster::~CRangedMonster() {}

void CRangedMonster::Init()
{
    collider = new CCollider();
    collider->SetScale(Vec2(45, 45));
    collider->SetLayer(Layer::Monster);
    AddChild(collider);

    animator = new CAnimator();

    // 오른쪽 이동: T_EyeMonster0 (40x40 / 3프레임, 가로 배치 가정)
    CImage* moveRight = LOADIMAGE(TEXT("T_EyeMonster0"), TEXT("Image\\T_EyeMonster0.bmp"));
    animator->CreateAnimation(TEXT("MoveRight"), moveRight,
        0.12f, 3, true,
        Vec2(0.f, 0.f),        // 첫 프레임 시작
        Vec2(40.f, 40.f),      // 프레임 크기
        Vec2(40.f, 0.f));      // 가로 stride

    // 왼쪽 이동: T_EyeMonster1 (40x40 / 3프레임, 가로 배치 가정)
    CImage* moveLeft = LOADIMAGE(TEXT("T_EyeMonster1"), TEXT("Image\\T_EyeMonster1.bmp"));
    animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
        0.12f, 3, true,
        Vec2(0.f, 0.f),
        Vec2(40.f, 40.f),
        Vec2(40.f, 0.f));

    AddChild(animator);
    animator->Play(TEXT("MoveRight"), true);
    animator->SetRatio(1.5f); // 150% 확대
}

void CRangedMonster::Update()
{
    // 이동
    CPlayer* p = GetPlayer();
    CombatStats& st = GetCombatStats();
    Vec2 dir(0.f, 0.f);

    if (p)
    {
        dir = p->GetWorldPos() - worldPos;
        float len = dir.Length();
        if (len > 0.0001f)
        {
            dir /= len;
            pos = pos + (dir * st.speed * DT);
        }
    }

    // 이동 방향에 따라 애니메이션 선택
    if (animator)
    {
        if (dir.x < -0.01f)
            animator->Play(TEXT("MoveLeft"), false);
        else
            animator->Play(TEXT("MoveRight"), false);
    }

    // 사거리 내면 발사 시도
    if (p)
    {
        Vec2 diff = p->GetWorldPos() - worldPos;
        if (diff.SqrMagnitude() <= attackRange * attackRange)
            TryFire();
    }

	UpdateStatusEffects();
}

void CRangedMonster::Render()
{
	RenderStatusEffects();
}

void CRangedMonster::TryFire()
{
    if (fireCooldown > 0.f)
    {
        fireCooldown -= DT;
        return;
    }

    CPlayer* p = GetPlayer();
    if (!p) return;

    Vec2 dir = p->GetWorldPos() - worldPos;
    float len = dir.Length();
    if (len < 1.f)
        return;
    dir /= len;

    SpawnMissile(dir);
    fireCooldown = fireInterval;
}

void CRangedMonster::SpawnMissile(const Vec2& dir)
{
    CombatStats& st = GetCombatStats();

    CMissile* m = new CMissile();
    m->SetPos(worldPos + dir * (scale.y * 0.5f + 6.f));
    m->SetDir(dir);
    m->SetFriendly(false);
    m->SetMoveSpeed(missileSpeed);
    m->SetLifeTime(5.f);

    // 전투 수치 전달
    CombatStats& ms = m->GetCombatStats();
    ms.attack         = st.attack;
    ms.defense        = st.defense;
    ms.critChance     = st.critChance;
    ms.critMultiplier = st.critMultiplier;

    EVENT->AddGameObject(GetScene(), m);
}

void CRangedMonster::Reset()
{
    CMonster::Reset();  // 부모 Reset 호출
    
    // CRangedMonster 고유 멤버 초기화
    CombatStats& st = GetCombatStats();
    st.hp             = 80.f;
    st.maxHp          = 80.f;
    st.attack         = 1.f;
    st.defense        = 0.f;
    st.critChance     = 0.00f;
    st.critMultiplier = 1.0f;
    st.speed          = 70.f;

    fireInterval  = 1.8f;
    fireCooldown  = 0.f;
    missileSpeed  = 100.f;
    attackRange   = 650.f;

    ExpValue = 20;
    ExpCount = 1;
}