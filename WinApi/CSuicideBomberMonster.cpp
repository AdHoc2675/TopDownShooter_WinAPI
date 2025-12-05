#include "pch.h"
#include "CSuicideBomberMonster.h"
#include "CPlayer.h"
#include "CExplosionEffect.h"

CSuicideBomberMonster::CSuicideBomberMonster()
{
    name = TEXT("자폭 몬스터");
    scale = Vec2(50, 50);
    CombatStats& st = GetCombatStats();
    st.hp = 70.f;
    st.maxHp = 70.f;
    st.attack = 1.f;
    st.speed = 100.f;
    
    ExpValue = 30;   // 경험치 보상
    ExpCount = 1;
}

CSuicideBomberMonster::~CSuicideBomberMonster()
{
}

void CSuicideBomberMonster::Init()
{
    collider = new CCollider();
    collider->SetScale(scale);
    collider->SetLayer(Layer::Monster);
    AddChild(collider);

    animator = new CAnimator();

    // 오른쪽 이동: T_Boomer0
    CImage* moveRight = LOADIMAGE(TEXT("T_Boomer0"), TEXT("Image\\T_Boomer0.bmp"));
    animator->CreateAnimation(TEXT("MoveRight"), moveRight,
        0.12f, 3, true,
        Vec2(0.f, 0.f),
        Vec2(32.f, 32.f),
        Vec2(32.f, 0.f));

    // 왼쪽 이동: T_Boomer1
    CImage* moveLeft = LOADIMAGE(TEXT("T_Boomer1"), TEXT("Image\\T_Boomer1.bmp"));
    animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
        0.12f, 3, true,
        Vec2(0.f, 0.f),
        Vec2(32.f, 32.f),
        Vec2(32.f, 0.f));

    AddChild(animator);
    animator->Play(TEXT("MoveRight"), true);
    animator->SetRatio(1.5f);
}

void CSuicideBomberMonster::Update()
{
    CPlayer* p = GetPlayer();
    if (!p)
        return;
    
    CombatStats& st = GetCombatStats();
    
    // 플레이어 방향 계산
    Vec2 toPlayer = p->GetWorldPos() - worldPos;
    float distance = toPlayer.Length();
    
    // 자폭 범위 체크 (이동 전에)
    if (distance <= detonationRange)
    {
        SelfDestruct();
        return;  // 자폭 후 더 이상 진행하지 않음
    }
    
    // 플레이어쪽으로 이동
    if (distance > 0.0001f)
    {
        Vec2 dir = toPlayer / distance;  // 정규화
        pos = pos + (dir * st.speed * DT);
        
        // 이동 방향에 따라 애니메이션 선택
        if (animator)
        {
            if (dir.x < -0.01f)
                animator->Play(TEXT("MoveLeft"), false);
            else
                animator->Play(TEXT("MoveRight"), false);
        }
    }
}

void CSuicideBomberMonster::OnCollisionEnter(CCollider* other)
{
    // 플레이어와 충돌 시 즉시 자폭
    if (other->GetLayer() == Layer::Player)
    {
        SelfDestruct();
        return;
    }
    
    CMonster::OnCollisionEnter(other);
}

void CSuicideBomberMonster::SelfDestruct()
{
    if (!GetScene())
        return;
    
    // 폭발 이펙트 생성
    CExplosionEffect* effect = new CExplosionEffect();
    effect->Configure(
        worldPos,           // 폭발 중심
        explosionRadius,    // 반경
        explosionDamage,    // 피해량
        false,              // 적군 폭발 (플레이어에 피해)
        0.5f                // 지속 시간
    );
    
    EVENT->AddGameObject(GetScene(), effect);
    
    // 경험치 드롭 (자폭 시에도)
    DropExpOrb(ExpValue, ExpCount);
    
    // 자신은 삭제
    EVENT->Delete(GetScene(), this);
}