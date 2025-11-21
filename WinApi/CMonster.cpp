#include "pch.h"
#include "CMonster.h"

CMonster::CMonster()
{
	name	= TEXT("몬스터");
	scale	= Vec2(100, 100);
}

CMonster::~CMonster()
{
}

void CMonster::Init()
{
    // 충돌 컴포넌트 추가
	CCollider* collider = new CCollider();
	collider->SetScale(Vec2(105, 105));
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
    curHitMsgTime = hitMsgDuration;
}

void CMonster::OnCollisionStay(CCollider* other)
{
}

void CMonster::OnCollisionExit(CCollider* other)
{
}