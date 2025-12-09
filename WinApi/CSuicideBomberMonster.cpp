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
    
    ExpValue = 30;
    ExpCount = 1;
    
    isCountingDown = false;
    countdownTimer = 0.f;
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
    
    // 카운트다운 중이면
    if (isCountingDown)
    {
        countdownTimer -= DT;
        
        // 카운트다운 종료 시 폭발
        if (countdownTimer <= 0.f)
        {
            SelfDestruct();
            return;
        }
        
        // 카운트다운 중에는 이동하지 않음 (여기서 리턴)
        return;
    }
    
    // 일반 이동 로직
    CombatStats& st = GetCombatStats();
    
    // 플레이어 방향 계산
    Vec2 toPlayer = p->GetWorldPos() - worldPos;
    float distance = toPlayer.Length();
    
    // 폭발 범위 진입 시 카운트다운 시작
    if (distance <= detonationRange)
    {
        StartDetonationCountdown();
        return;
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

	UpdateStatusEffects();
}

void CSuicideBomberMonster::Render()
{
    // 카운트다운 중이면 경고 표시
    if (isCountingDown)
    {
        // 깜빡이는 붉은 원 (경고 표시)
        float pulseFreq = 8.0f; // 초당 8회 깜빡임
        float pulseValue = sinf((countdownTime - countdownTimer) * pulseFreq * 3.14159f * 2.0f);
        pulseValue = (pulseValue + 1.0f) * 0.5f; // 0~1 범위로 변환
        
        // 경고 원 그리기
        COLORREF warningColor = RGB(255, (int)(100 * pulseValue), 0);
        RENDER->SetPen(PenType::Solid, warningColor, 3);
        RENDER->SetBrush(BrushType::Null);
        
        float warningRadius = 40.f + pulseValue * 10.f; // 40~50 범위로 맥동
        RENDER->Circle(renderPos.x, renderPos.y, warningRadius);
        
        // 카운트다운 시간 표시 (선택사항)
        int timeLeft = (int)(countdownTimer * 10); // 0.1초 단위
        wstring countText = L"!";
        
        RENDER->SetText(20, RGB(255, 0, 0), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(renderPos.x, renderPos.y - 60.f, countText);
    }

	RenderStatusEffects();
}

void CSuicideBomberMonster::OnCollisionEnter(CCollider* other)
{
    // 플레이어와 충돌 시 즉시 카운트다운 시작 (즉시 폭발 대신)
    if (other->GetLayer() == Layer::Player)
    {
        if (!isCountingDown)
        {
            StartDetonationCountdown();
        }
        return;
    }
    
    CMonster::OnCollisionEnter(other);
}

void CSuicideBomberMonster::StartDetonationCountdown()
{
    if (isCountingDown)
        return; // 이미 카운트다운 중
    
    isCountingDown = true;
    countdownTimer = countdownTime;
    
    Logger::Debug(L"[CSuicideBomberMonster] Detonation countdown started!");
}

void CSuicideBomberMonster::SelfDestruct()
{
    if (!GetScene())
        return;
    
    Logger::Debug(L"[CSuicideBomberMonster] Self-destruct!");
    
    CExplosionEffect* effect = new CExplosionEffect();
    effect->Configure(worldPos, explosionRadius, explosionDamage, false, 0.5f);
    EVENT->AddGameObject(GetScene(), effect);
    
    DropExpOrb(ExpValue, ExpCount);
    droppedExpOrb = true;
    
    EVENT->Delete(GetScene(), this);

    // 풀 객체면 풀로 반환, 아니면 삭제
    //if (fromPool)
    //{
    //    ReturnToPool();
    //}
    //else
    //{
    //    EVENT->Delete(GetScene(), this);
    //}
}

//// Reset 함수 추가
//void CSuicideBomberMonster::Reset()
//{
//    CMonster::Reset();  // 부모 Reset 호출
//    
//    // CSuicideBomberMonster 고유 멤버 초기화
//    CombatStats& st = GetCombatStats();
//    st.hp = 70.f;
//    st.maxHp = 70.f;
//    st.attack = 1.f;
//    st.speed = 100.f;
//    
//    ExpValue = 30;
//    ExpCount = 1;
//    
//    isCountingDown = false;
//    countdownTimer = 0.f;
//    
//    detonationRange = 70.f;
//    explosionRadius = 100.f;
//    explosionDamage = 1.f;
//    countdownTime = 0.75f;
//}