#include "pch.h"
#include "CExpOrb.h"
#include "CPlayer.h"
#include "CCollider.h"

using namespace std;

CExpOrb::CExpOrb()
{
    name  = TEXT("경험치구슬");
    scale = Vec2(16.f, 16.f);

    int      value = 5;      // 획득 경험치
    float    attractRadius = 250.f;
    float    moveSpeed = 220.f;
    float    homingSpeed = 400.f;
    bool     isHoming = false;
}

CExpOrb::~CExpOrb()
{
}

void CExpOrb::Init()
{
    CCollider* col = new CCollider();
    col->SetScale(scale);
    col->SetLayer(Layer::ExpOrb); // Layer::ExpOrb 추가 필요
    AddChild(col);
}

void CExpOrb::OnEnable()
{
}

void CExpOrb::Update()
{
    if (!player)
        return;

    Vec2 toPlayer = player->GetWorldPos() - worldPos;
    float dist = toPlayer.Length();

    if (!isHoming && dist < attractRadius)
    {
        isHoming = true;
    }

    if (isHoming)
    {
        toPlayer.Normalize();
        pos = pos + toPlayer * homingSpeed * DT;
    }
    else
    {
        // 간단한 부유(선회) 효과를 원하면 여기에 추가 가능
        // pos.y += sinf((float)GetTickCount() * 0.005f) * 20.f * DT;
    }
}

void CExpOrb::Render()
{
    RENDER->SetPen(PenType::Solid, RGB(0, 150, 0), 1);
    RENDER->SetBrush(PenType::Solid == PenType::Solid ? BrushType::Solid : BrushType::Solid, RGB(100, 255, 100));

    RENDER->Ellipse(
        renderPos.x - scale.x * 0.5f,
        renderPos.y - scale.y * 0.5f,
        renderPos.x + scale.x * 0.5f,
        renderPos.y + scale.y * 0.5f);

    // 값 표시(디버그)
    RENDER->SetText(10, RGB(0, 60, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(renderPos.x, renderPos.y - scale.y * 0.5f - 12.f, L"+" + to_wstring(value));
}

void CExpOrb::OnDisable()
{
}

void CExpOrb::Release()
{
}

void CExpOrb::OnCollisionEnter(CCollider* other)
{
    if (other->GetLayer() == Layer::Player && player)
    {
        player->AddExp(value);
        EVENT->Delete(GetScene(), this);
    }
}