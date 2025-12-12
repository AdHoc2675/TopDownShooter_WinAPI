#include "pch.h"
#include "CPlayerDiamond.h"
#include "CAnimator.h"

CPlayerDiamond::CPlayerDiamond()
{
    name = TEXT("플레이어 - 다이아몬드");
    scale = Vec2(25, 40);
    animator = nullptr;
    heartFullImage = nullptr;
    heartEmptyImage = nullptr;

    stats.speed  = 150.f;
    stats.maxHp  = 7.f;
    stats.hp     = 7.f;
    stats.attack = 20.f;
    stats.critChance = 0.f;
    stats.critMultiplier = 1.5f;
    hitCooldown = 0.f;

    level = 1;
    exp = 0;
    maxExp = 50;
    moveDir = Vec2(0, 0);
    lookDir = Vec2(0, -1);
    isMove = false;

    // 발소리 초기화 (추가)
    footstepInterval = 0.35f;
    footstepTimer = 0.f;
    footstepSounds[0] = footstepSounds[1] = footstepSounds[2] = nullptr;
}

CPlayerDiamond::~CPlayerDiamond()
{
}

void CPlayerDiamond::Init()
{
    heartEmptyImage = LOADIMAGE(TEXT("HeartEmpty"), TEXT("Image\\CPlayer_hpEmpty.bmp"));
    heartFullImage = LOADIMAGE(TEXT("HeartFull"), TEXT("Image\\CPlayer_hpFull.bmp"));

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

    animator->SetRatio(2.0f);
    AddChild(animator);

    // 충돌 컴포넌트 추가
    CCollider* collider = new CCollider();
    collider->SetScale(scale);
    collider->SetLayer(Layer::Player);
    AddChild(collider);

    // 발소리 로드
    footstepSounds[0] = LOADSOUND(TEXT("Footsteps_Casual_Grass_01"), TEXT("Sound\\Footsteps_Casual_Grass_01.wav"));
    footstepSounds[1] = LOADSOUND(TEXT("Footsteps_Casual_Grass_02"), TEXT("Sound\\Footsteps_Casual_Grass_02.wav"));
    footstepSounds[2] = LOADSOUND(TEXT("Footsteps_Casual_Grass_03"), TEXT("Sound\\Footsteps_Casual_Grass_03.wav"));
    // 피격음 로드
    hitSound = LOADSOUND(TEXT("Player_Hit"), TEXT("Sound\\Blood_Splash_Quick_01.wav"));
    // 경험치 획득음 로드
    addExpSound = LOADSOUND(TEXT("Exp_Gain"), TEXT("Sound\\Coins (10).wav"));
}

void CPlayerDiamond::OnEnable()
{
    CPlayer::OnEnable();
}

void CPlayerDiamond::Update()
{
#pragma region 이동 입력 처리

    isMove = false;

    // 이동
    if (INPUT->ButtonStay('A', true))
    {
        pos.x -= stats.speed * DT;
        isMove = true;
        moveDir.x = -1;
    }
    else if (INPUT->ButtonStay('D', true))
    {
        pos.x += stats.speed * DT;
        isMove = true;
        moveDir.x = +1;
    }
    else
    {
        moveDir.x = 0;
    }

    if (INPUT->ButtonStay('W', true))
    {
        pos.y -= stats.speed * DT;
        isMove = true;
        moveDir.y = +1;
    }
    else if (INPUT->ButtonStay('S', true))
    {
        pos.y += stats.speed * DT;
        isMove = true;
        moveDir.y = -1;
    }
    else
    {
        moveDir.y = 0;
    }

#pragma endregion

    // 피해 쿨다운 감소
    if (hitCooldown > 0.f)
        hitCooldown = hitCooldown - DT;
    if (hitCooldown < 0.f)
        hitCooldown = 0.f;

    // 발소리 재생
    if (isMove)
    {
        footstepTimer -= DT;
        if (footstepTimer <= 0.f)
        {
            int idx = rand() % 3;
            if (footstepSounds[idx])
                SOUND->PlayOnce(footstepSounds[idx]);
            footstepTimer = footstepInterval;
        }
    }
    else
    {
        // 멈추면 타이머를 0으로 두어, 다시 움직일 때 즉시 한 번 재생되도록
        footstepTimer = 0.f;
    }

    // 경험치 테스트용
    if (INPUT->ButtonDown('T', true)) {
        AddExp(maxExp / 2);
    }

    AnimatorUpdate();

    // 대시 쿨다운 처리
    if (dashCooldown > 0.f) dashCooldown -= DT;
    if (INPUT->ButtonDown(VK_SHIFT, true) && dashCooldown <= 0.f)
    {
        // 대시 로직
        pos.x = pos.x + moveDir.x * 100.f;
        pos.y = pos.y - moveDir.y * 100.f;
        dashCooldown = 1.0f;
    }
}

void CPlayerDiamond::Release()
{
    CPlayer::Release();
}