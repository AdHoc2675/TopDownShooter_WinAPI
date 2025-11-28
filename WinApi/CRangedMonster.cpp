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
    st.hp             = 40.f;
    st.maxHp          = 40.f;
    st.attack         = 1.f;
    st.defense        = 0.f;
    st.critChance     = 0.05f;
    st.critMultiplier = 1.5f;
    st.speed          = 70.f;   // 기본 몬스터보다 느림

    fireInterval  = 1.8f;
    fireCooldown  = 0.f;
    missileSpeed  = 220.f;       // 느린 투사체
    attackRange   = 650.f;       // 사거리
}

CRangedMonster::~CRangedMonster() {}

void CRangedMonster::Init()
{
    CCollider* collider = new CCollider();
    collider->SetScale(Vec2(45, 45));
    collider->SetLayer(Layer::Monster);
    AddChild(collider);
}

void CRangedMonster::Update()
{
    static float hitMsgDuration = 0.4f;
    static float curHitMsgTime = 0.f;
    if (curHitMsgTime > 0.f)
    {
        curHitMsgTime -= DT;
        if (curHitMsgTime < 0.f) curHitMsgTime = 0.f;
    }

    // 이동
    CPlayer* p = GetPlayer();
    CombatStats& st = GetCombatStats();
    if (p)
    {
        Vec2 dir = p->GetWorldPos() - worldPos;
        float len = dir.Length();
        if (len > 0.0001f)
        {
            dir /= len;
            pos = pos + (dir * st.speed * DT);
        }
    }

    // 사거리 내면 발사 시도
    if (p)
    {
        Vec2 diff = p->GetWorldPos() - worldPos;
        if (diff.SqrMagnitude() <= attackRange * attackRange)
            TryFire();
    }
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