#include "pch.h"
#include "CMonster.h"
#include "CPlayer.h"
#include "CMissile.h"
#include "CCombatSystem.h"
#include "CCollider.h"
#include "CExpOrb.h"

CMonster::CMonster() 
{
    name  = TEXT("몬스터");
    scale = Vec2(40, 40);
    stats.hp       = 60.f;
    stats.maxHp    = 60.f;
    stats.defense  = 0.f;
    stats.attack = 1.f;
    stats.critChance = 0.f;
    stats.critMultiplier = 1.0f;

    speed = 100.f;
    hitMsgDuration = 0.4f;
    curHitMsgTime = 0.f;
	droppedExpOrb = false;

    hitMsg = L"Hit!";
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
}

void CMonster::OnEnable()
{
}

void CMonster::Update()
{
    if (curHitMsgTime > 0.f)
    {
        curHitMsgTime = curHitMsgTime - DT;
        if (curHitMsgTime < 0.f) {
            curHitMsgTime = 0.f;
        }
    }

	// 플레이어 쪽으로 이동
    if (player != nullptr)
    {
        Vec2 dir = player->GetWorldPos() - worldPos;
        dir.Normalize();
        pos = pos + (dir * speed * DT);
	}
}

void CMonster::Render()
{
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
    RENDER->SetBrush(BrushType::Solid, RGB(255, 255, 255));

    RENDER->Rect(
        renderPos.x - scale.x * 0.5f,
        renderPos.y - scale.y * 0.5f,
        renderPos.x + scale.x * 0.5f,
        renderPos.y + scale.y * 0.5f);

	//=====//

    // 피격 메시지
    int textSize = 12;
    if (curHitMsgTime > 0.f)
    {
        // 피격 메시지 출력
        RENDER->SetText(textSize, RGB(255, 0, 0), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(
            renderPos.x,
            renderPos.y - scale.y * 0.5f - 20.f,
            hitMsg);
	}
}

void CMonster::OnDisable()
{
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

void CMonster::OnCollisionEnter(CCollider* other)
{
    if (other->GetLayer() == Layer::Missile)
    {
        CGameObject* missileObj = other->GetOwner();
        CMissile* missile = dynamic_cast<CMissile*>(missileObj);
        if (missile)
        {
            CombatStats& attackerStats = missile->GetCombatStats();
            COMBAT->ApplyDamage(missile, this, attackerStats, stats);
            curHitMsgTime = hitMsgDuration;
            hitMsg = L"-" + to_wstring((int)attackerStats.attack);

            if (stats.alive() == false && droppedExpOrb == false)
            {
                DropExpOrb();
				droppedExpOrb = true;
            }
        }
    }
}

void CMonster::OnCollisionStay(CCollider* other)
{
    if (other->GetLayer() != Layer::Monster)
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
        Vec2 n = diff / dist;               // 정규화 방향
        Vec2 correction = n * (overlap * 0.5f);

        // 두 몬스터를 반씩 이동
        pos += correction;
        otherMonster->SetPos(otherMonster->GetPos() - correction);
    }
}

void CMonster::OnCollisionExit(CCollider* other)
{
}
