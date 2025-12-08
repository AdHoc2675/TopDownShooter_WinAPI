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
#include "CMonsterPoolManager.h"

CMonster::CMonster() 
{
    name  = TEXT("몬스터");
    scale = Vec2(40, 40);
    st.hp       = 80.f;
    st.maxHp    = 80.f;
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

    UpdateStatusEffects();

}

void CMonster::Render()
{
    RenderStatusEffects();
}

void CMonster::OnDisable()
{
    CScene* s = GetScene();
    CSceneStage01* stage = dynamic_cast<CSceneStage01*>(s);
    if (stage)
        stage->UnregisterMonster(this);

    // 풀링 객체면 추가 처리 없음 (ReturnToPool에서 처리)
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

// OnCollisionEnter 함수 - 미사일 처리 부분에 발화 적용 추가
void CMonster::OnCollisionEnter(CCollider* other)
{
    if (other->GetLayer() == Layer::Missile)
    {
        CGameObject* attackerObj = other->GetOwner();

        // 1) 투사체 처리
        if (CMissile* missile = dynamic_cast<CMissile*>(attackerObj))
        {
            if (!missile->GetFriendly())
                return;

            CombatStats& attackerStats = missile->GetCombatStats();

            float dealt = 0.f;
            bool  crit = false;
            COMBAT->ApplyDamage(missile, this, attackerStats, st, &dealt, &crit);

            // 발화 적용 (확률 체크 추가)
            if (missile->GetAppliesBurn())
            {
                float roll = (float)(rand() % 100) / 100.f;  // 0.0 ~ 0.99
                if (roll < missile->GetBurnChance())
                {
                    ApplyStatusEffect(StatusEffectType::Burn,
                        missile->GetBurnStacks(),
                        missile->GetBurnDuration());
                }
            }

            // 피격 이펙트 텍스트
            CDamageText* dt = new CDamageText();
            dt->Configure(worldPos, (int)dealt, crit);
            EVENT->AddGameObject(GetScene(), dt);

            if (!st.alive() && !droppedExpOrb)
            {
                DropExpOrb(ExpValue, ExpCount);
                droppedExpOrb = true;

                // 처치 카운트 증가
                CScene* s = GetScene();
                CSceneStage01* stage = dynamic_cast<CSceneStage01*>(s);
                if (stage)
                    stage->AddMonsterKill();
            }
            return;
        }

        // 2) 소환수(CScythe) 처리 - 기존 코드 그대로 유지
        if (CScythe* scythe = dynamic_cast<CScythe*>(attackerObj))
        {
            CombatStats attackerStats = scythe->GetCombatStats();

            float dealt = 0.f;
            bool  crit = false;
            COMBAT->ApplyDamage(scythe, this, attackerStats, st, &dealt, &crit);

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

void CMonster::Reset()
{
    // 전투 스탯 초기화
    st.hp       = 80.f;
    st.maxHp    = 80.f;
    st.defense  = 0.f;
    st.attack   = 1.f;
    st.critChance = 0.f;
    st.critMultiplier = 1.0f;
    st.speed    = 100.f;

    droppedExpOrb = false;
    ExpValue = 15;
    ExpCount = 1;
    
    // 삭제 예정 플래그 초기화
    reservedDelete = false;
    
    pos = Vec2(0, 0);
    worldPos = Vec2(0, 0);
    player = nullptr;

    ClearAllStatusEffects();
    
    if (animator)
    {
        animator->Play(TEXT("MoveRight"), true);
    }
}

void CMonster::ReturnToPool()
{
    CScene* s = GetScene();
    
    // 1. 씬의 몬스터 목록에서 제거
    if (s)
    {
        CSceneStage01* stage = dynamic_cast<CSceneStage01*>(s);
        if (stage)
        {
            stage->UnregisterMonster(this);
        }
    }
    
    // 2. 풀에서 온 객체면 풀로 반환
    if (fromPool)
    {
        // 씬에서 제거 (삭제 대신)
        if (s)
        {
            s->RemoveGameObject(this);
        }
        
        // 풀로 반환
        MONSTERPOOL->ReleaseMonster(this);
    }
    else
    {
        // 풀링되지 않은 객체는 기존 방식으로 삭제
        EVENT->Delete(s, this);
    }
}

void CMonster::ApplyStatusEffect(StatusEffectType type, int stacks, float duration)
{
    // 이미 같은 타입의 상태이상이 있는지 확인
    for (auto& effect : statusEffects)
    {
        if (effect.type == type)
        {
            // 중첩 추가 및 지속시간 갱신
            effect.stacks += stacks;
            
            if (effect.stacks > effect.maxStacks)
				effect.stacks = effect.maxStacks;

            if (effect.duration < duration)
                effect.duration = duration;
            return;
        }
    }

    // 새로운 상태이상 추가
    StatusEffect newEffect;
    newEffect.type = type;
    newEffect.stacks = stacks;
    newEffect.duration = duration;

    switch (type)
    {
    case StatusEffectType::Burn:
        newEffect.tickInterval = 1.0f;  // 1초마다 틱
        newEffect.tickTimer = 1.0f;
        newEffect.damagePerStack = 3.0f; // 중첩당 3 피해
        newEffect.maxStacks = BURN_MAX_STACKS;
        break;
    }

    if (newEffect.stacks > newEffect.maxStacks)
        newEffect.stacks = newEffect.maxStacks;

    statusEffects.push_back(newEffect);
}

void CMonster::RemoveStatusEffect(StatusEffectType type)
{
    statusEffects.erase(
        std::remove_if(statusEffects.begin(), statusEffects.end(),
            [type](const StatusEffect& e) { return e.type == type; }),
        statusEffects.end());
}

bool CMonster::HasStatusEffect(StatusEffectType type) const
{
    for (const auto& effect : statusEffects)
    {
        if (effect.type == type)
            return true;
    }
    return false;
}

int CMonster::GetStatusEffectStacks(StatusEffectType type) const
{
    for (const auto& effect : statusEffects)
    {
        if (effect.type == type)
            return effect.stacks;
    }
    return 0;
}

void CMonster::ClearAllStatusEffects()
{
    statusEffects.clear();
}

void CMonster::UpdateStatusEffects()
{
    // 몬스터가 죽었으면 모든 상태이상 제거 후 리턴
    if (!st.alive())
    {
        ClearAllStatusEffects();
        return;
    }

    // 역순으로 순회하여 안전하게 삭제
    for (int i = (int)statusEffects.size() - 1; i >= 0; --i)
    {
        StatusEffect& effect = statusEffects[i];

        // 지속시간 감소
        effect.duration -= DT;
        effect.tickTimer -= DT;

        // 틱 발생
        if (effect.tickTimer <= 0.f)
        {
            effect.tickTimer += effect.tickInterval;

            // 피해 계산 및 적용
            float damage = effect.damagePerStack * effect.stacks;
            st.hp -= damage;

            // 피해 텍스트 표시
            CDamageText* dt = new CDamageText();
            dt->Configure(worldPos + Vec2(0, -20.f), (int)damage, false);
            EVENT->AddGameObject(GetScene(), dt);

            // 사망 체크
            if (!st.alive() && !droppedExpOrb)
            {
                DropExpOrb(ExpValue, ExpCount);
                droppedExpOrb = true;

                // 처치 카운트 증가
                CScene* s = GetScene();
                CSceneStage01* stage = dynamic_cast<CSceneStage01*>(s);
                if (stage)
                    stage->AddMonsterKill();

                ClearAllStatusEffects();

                if (fromPool)
                {
                    ReturnToPool();
                }
                else
                {
                    EVENT->Delete(s, this);
                }
                return;
                return;
            }
        }

        // 지속시간 종료 시 제거 (틱 체크 후에 수행)
        if (effect.duration <= 0.f)
        {
            statusEffects.erase(statusEffects.begin() + i);
            continue;
        }
    }
}

void CMonster::RenderStatusEffects()
{
    if (statusEffects.empty())
        return;

    // 죽은 몬스터는 상태이상 아이콘 표시 안 함
    if (!st.alive())
        return;

    float iconY = renderPos.y - scale.y * 0.5f - 15.f;
    float iconX = renderPos.x;

    for (const auto& effect : statusEffects)
    {
        // 지속시간이 남아있을 때만 렌더링
        if (effect.duration <= 0.f)
            continue;

        if (effect.type == StatusEffectType::Burn)
        {
            // 발화 아이콘 (불꽃 모양 원)
            float pulseValue = sinf(effect.duration * 5.f) * 0.2f + 0.8f;
            float iconSize = 8.f * pulseValue;

            RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
            RENDER->SetBrush(BrushType::Solid, RGB(255, 100, 0));
            RENDER->Circle(iconX, iconY, iconSize);

            iconX += 18.f; // 다음 아이콘 위치
        }
    }
}