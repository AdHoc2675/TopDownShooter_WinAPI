#include "pch.h"
#include "CExpOrb.h"
#include "CPlayer.h"
#include "CCollider.h"

using namespace std;

CExpOrb::CExpOrb()
{
    name  = TEXT("경험치구슬");
    scale = Vec2(16.f, 16.f);

    value = 15;      // 획득 경험치
    attractRadius = 150.f;
    moveSpeed = 220.f;
    homingSpeed = 400.f;
    isHoming = false;
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
	CExpOrb_image1 = LOADIMAGE(TEXT("ExpOrb1"), TEXT("Image\\ExpOrb1.bmp"));
    CExpOrb_image2 = LOADIMAGE(TEXT("ExpOrb2"), TEXT("Image\\ExpOrb2.bmp"));
    CExpOrb_image3 = LOADIMAGE(TEXT("ExpOrb3"), TEXT("Image\\ExpOrb3.bmp"));

    if (value >= 100) {
        CurrentImage = CExpOrb_image3;
    }
    else if (value >= 30) {
        CurrentImage = CExpOrb_image2;
    }
    else {
        CurrentImage = CExpOrb_image1;
	}
}

void CExpOrb::OnEnable(){}

void CExpOrb::Update()
{
    if (!player)
        return;

    Vec2 toPlayer = player->GetWorldPos() - worldPos;
    float dist = toPlayer.Length();

    if (isHoming == false && dist < attractRadius)
    {
        isHoming = true;
    }

    if (isHoming == true)
    {
        toPlayer.Normalize();
        pos = pos + toPlayer * homingSpeed * DT;
    }
    else
    {

    }
}

void CExpOrb::Render()
{
    if (CurrentImage) {
        RENDER->TransparentImage(
            CurrentImage,
            renderPos.x - scale.x * 0.5f,
            renderPos.y - scale.y * 0.5f,
            renderPos.x + scale.x * 0.5f,
            renderPos.y + scale.y * 0.5f,
            RGB(255, 0, 255));

    }

    // 경험치 값 표시
    int textSize = 10;
    RENDER->SetText(textSize, RGB(0, 60, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(renderPos.x, renderPos.y - scale.y * 0.5f - (float)textSize, L"+" + to_wstring(value));
}

void CExpOrb::OnDisable()
{
}

void CExpOrb::Release()
{
}

void CExpOrb::UpdateImage()
{
    if (value >= 100) {
        CurrentImage = CExpOrb_image3;
    }
    else if (value >= 30) {
        CurrentImage = CExpOrb_image2;
    }
    else {
        CurrentImage = CExpOrb_image1;
    }
}

void CExpOrb::OnCollisionEnter(CCollider* other)
{
    if (other->GetLayer() == Layer::Player && player)
    {
        player->AddExp(value);
        EVENT->Delete(GetScene(), this);
    }
}