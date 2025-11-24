#include "pch.h"
#include "CMonster.h"
#include "CPlayer.h"
#include "CMissile.h"
#include "CCombatSystem.h"
#include "CCollider.h"

CMonster::CMonster()
{
    name  = TEXT("몬스터");
    scale = Vec2(40, 40);
    stats.hp       = 60.f;
    stats.maxHp    = 60.f;
    stats.defense  = 2.f;
    stats.attack   = 8.f;      // (사용할지 선택)
    stats.critChance = 0.05f;
    stats.critMultiplier = 1.4f;

    speed = 100.f;
    hitMsgDuration = 0.4f;
    curHitMsgTime = 0.f;

    hitMsg = L"Hit!";
}

CMonster::~CMonster()
{
}

void CMonster::Init()
{
    // 충돌 컴포넌트 추가
	CCollider* collider = new CCollider();
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
        }
    }
}
