#include "pch.h"
#include "CPlayerDiamond.h"
#include "CAnimator.h"

CPlayerDiamond::CPlayerDiamond()
{
    name = TEXT("플레이어 - 다이아몬드");
    // 기본 스탯 오버라이드
    GetCombatStats().attack = 20.f;
    GetCombatStats().maxHp  = 5.f;
    GetCombatStats().hp     = 5.f;
    GetCombatStats().speed  = 150.f;
}

CPlayerDiamond::~CPlayerDiamond()
{
}

void CPlayerDiamond::Init()
{
    CPlayer::Init(); // 기본 초기화 유지(충돌체 등)

    // 애니메이터/이미지 로드
    CImage* rightImage = LOADIMAGE(TEXT("PlayerRightDiamond"), TEXT("Image\\T_Diamond0.bmp"));
    CImage* leftImage = LOADIMAGE(TEXT("PlayerLeftDiamond"), TEXT("Image\\T_Diamond1.bmp"));
    animator = new CAnimator();

    animator->CreateAnimation(TEXT("IdleRight"), rightImage,
        0.1f, 6, true,
        Vec2(0.f, 0.f),       // 첫 프레임 시작 위치
        Vec2(32.f, 32.f),     // 프레임 크기
        Vec2(32.f, 0.f));     // 프레임 간 이동(가로)

    animator->CreateAnimation(TEXT("IdleLeft"), leftImage,
        0.1f, 6, true,
        Vec2(128.f, 0.f),
        Vec2(32.f, 32.f),
        Vec2(32.f, 0.f));

    // MoveRight / MoveLeft (16프레임, 가로 배치 가정)
    animator->CreateAnimation(TEXT("MoveRight"), rightImage,
        0.1f, 4, true,
        Vec2(0.f, 32.f),
        Vec2(32.f, 32.f),
        Vec2(32.f, 0.f));

    animator->CreateAnimation(TEXT("MoveLeft"), leftImage,
        0.1f, 4, true,
        Vec2(192.f, 32.f),
        Vec2(32.f, 32.f),
        Vec2(32.f, 0.f));

    AddChild(animator);
    animator->Play(TEXT("Idle"), true);
}

void CPlayerDiamond::OnEnable()
{
    CPlayer::OnEnable();
}

void CPlayerDiamond::Update()
{
    CPlayer::Update();

    // 대시 쿨다운 처리
    if (dashCooldown > 0.f) dashCooldown -= DT;
    if (INPUT->ButtonDown(VK_SHIFT, true) && dashCooldown <= 0.f)
    {
        // 대시 로직 (간단 예시)
        pos.x = pos.x + moveDir.x * 100.f;
        pos.y = pos.y + moveDir.y * 100.f;
        dashCooldown = 1.0f;
    }
}

void CPlayerDiamond::Release()
{
    CPlayer::Release();
}