#include "pch.h"
#include "CMonster.h"
#include "CPlayer.h"
#include "CMissile.h"
#include "CCombatSystem.h"
#include "CCollider.h"
#include "CExpOrb.h"
#include "CSceneStage01.h"
#include "CAnimator.h"
#include "CScythe.h"
#include "CDamageText.h"

CMonster::CMonster() 
{
    name  = TEXT("몬스터");
    scale = Vec2(40, 40);
    st.hp       = 100.f;
    st.maxHp    = 100.f;
    st.defense  = 0.f;
    st.attack = 1.f;
    st.critChance = 0.f;
    st.critMultiplier = 1.0f;
    st.speed = 100.f;

	droppedExpOrb = false;
    ExpValue = 15;
	ExpCount = 1;

}

CMonster::~CMonster()
{
}

void CMonster::Init()
{
    collider = new CCollider();
    collider->SetScale(Vec2(45, 45));
    collider->SetLayer(Layer::Monster);
    AddChild(collider);

    animator = new CAnimator();

    // 우측 이동: T_GhostMonster0 (64x64 / 7프레임, 가로로 배치 가정)
    CImage* moveRight = LOADIMAGE(TEXT("T_GhostMonster0"), TEXT("Image\\T_GhostMonster0.bmp"));
    animator->CreateAnimation(TEXT("MoveRight"), moveRight,
        0.1f, 7, true,
        Vec2(0.f, 0.f),        // 첫 프레임 시작 위치
        Vec2(64.f, 64.f),      // 프레임 크기
        Vec2(64.f, 0.f));      // 프레임 간 이동(가로)

    // 좌측 이동: T_GhostMonster1 (64x64 / 7프레임)
    CImage* moveLeft = LOADIMAGE(TEXT("T_GhostMonster1"), TEXT("Image\\T_GhostMonster1.bmp"));
    animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
        0.1f, 7, true,
        Vec2(0.f, 0.f),
        Vec2(64.f, 64.f),
        Vec2(64.f, 0.f));

    AddChild(animator);
    animator->Play(TEXT("MoveRight"), true);
    animator->SetRatio(1.5f); // 150% 확대
}

void CMonster::OnEnable()
{
}

void CMonster::Update()
{
	// 플레이어 쪽으로 이동
    Vec2 dir(0.f, 0.f);

    if (player != nullptr)
    {
        dir = player->GetWorldPos() - worldPos;
        dir.Normalize();
        pos = pos + (dir * st.speed * DT);
	}

    if (animator)
    {
        if (dir.x < -0.01f)
            animator->Play(TEXT("MoveLeft"), false);
        else
            animator->Play(TEXT("MoveRight"), false);
    }

}

void CMonster::Render()
{

}

void CMonster::OnDisable()
{
    CScene* s = GetScene();
    CSceneStage01* stage = dynamic_cast<CSceneStage01*>(s);
    if (stage)
        stage->UnregisterMonster(this);

}

void CMonster::Release()
{
}

void CMonster::DropExpOrb()
{
    if (player == nullptr)
		return;

    int dropCount = 1;

    for (int i = 0 ; i < dropCount; ++i)
    {
        CExpOrb* expOrb = new CExpOrb();

        expOrb->SetPos(worldPos);
        expOrb->SetPlayer(player);
        EVENT->AddGameObject(GetScene(), expOrb);
	}
}

void CMonster::DropExpOrb(int value, int count)
{
    if (player == nullptr|| count == 0)
        return;
    for (int i = 0 ; i < count; i++)
    {
        CExpOrb* expOrb = new CExpOrb();
        expOrb->SetPos(worldPos);
        expOrb->SetPlayer(player);
        expOrb->SetValue(value);
        EVENT->AddGameObject(GetScene(), expOrb);
	}
}

void CMonster::OnCollisionEnter(CCollider* other)
{
    if (other->GetLayer() == Layer::Missile)
    {
        CGameObject* attackerObj = other->GetOwner();

        // 1) 투사체 처리
        if (CMissile* missile = dynamic_cast<CMissile*>(attackerObj))
        {
            // 아군(플레이어/터렛) 미사일만 몬스터에 피해 적용
            if (!missile->GetFriendly())
                return;

            CombatStats& attackerStats = missile->GetCombatStats();

            float dealt = 0.f;
            bool  crit  = false;
            COMBAT->ApplyDamage(missile, this, attackerStats, st, &dealt, &crit);

            // 피격 이펙트 텍스트
            CDamageText* dt = new CDamageText();
            dt->Configure(worldPos, (int)dealt, crit);
            EVENT->AddGameObject(GetScene(), dt);

            if (!st.alive() && !droppedExpOrb)
            {
                DropExpOrb(ExpValue, ExpCount);
                droppedExpOrb = true;
            }
            return;
        }

        // 2) 소환수(CScythe) 처리
        if (CScythe* scythe = dynamic_cast<CScythe*>(attackerObj))
        {
            // 플레이어 소유 소환수는 항상 적 몬스터에 피해 적용
            CombatStats attackerStats = scythe->GetCombatStats(); // 복사 또는 참조 제공 함수가 있으면 교체

            float dealt = 0.f;
            bool  crit  = false;
            COMBAT->ApplyDamage(scythe, this, attackerStats, st, &dealt, &crit);

            // 피격 이펙트 텍스트
            CDamageText* dt = new CDamageText();
            dt->Configure(worldPos, (int)dealt, crit);
            EVENT->AddGameObject(GetScene(), dt);

            if (!st.alive() && !droppedExpOrb)
            {
                DropExpOrb(ExpValue, ExpCount);
                droppedExpOrb = true;
            }
            return;
        }

    }
}

void CMonster::OnCollisionStay(CCollider* other)
{
    if (other->GetLayer() != Layer::Monster)
        return;

    if (collider == nullptr || other == nullptr)
		return;

    // 중복 분리 방지: collider ID가 더 작은 쪽만 처리
    if (collider->GetID() >= other->GetID())
        return;

    CGameObject* otherObj = other->GetOwner();
    CMonster* otherMonster = dynamic_cast<CMonster*>(otherObj);
    if (!otherMonster) return;

    // 위치
    Vec2 aPos = worldPos;
    Vec2 bPos = otherObj->GetWorldPos();

    // 반경(대략 절반 폭)
    float aRadius = collider->GetScale().x * 0.5f;
    float bRadius = other->GetScale().x * 0.5f;

    Vec2 diff = aPos - bPos;
    float dist = diff.Length();
    if (dist <= 0.0001f)
    {
        diff = Vec2(1.f, 0.f);
        dist = 1.f;
    }

    float targetDist = aRadius + bRadius;
    float overlap = targetDist - dist;
    if (overlap > 0.f)
    {
        Vec2 n = diff / dist;   // 정규화 방향
        Vec2 correction = n * (overlap * 0.5f);

        // 두 몬스터를 반씩 이동
        pos += correction;
        otherMonster->SetPos(otherMonster->GetPos() - correction);
    }
}

void CMonster::OnCollisionExit(CCollider* other)
{
}
