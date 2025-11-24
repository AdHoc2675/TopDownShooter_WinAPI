#include "pch.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CCombatSystem.h"

CPlayer::CPlayer()
{
	name		= TEXT("플레이어");
	scale		= Vec2(100, 100);
	animator	= nullptr;
	speed		= 200.f;

	stats.hp = 5.f;
	stats.maxHp = 5.f;
	stats.defense = 0.f;
	stats.attack = 15.f;
	stats.critChance = 0.f;
	stats.critMultiplier = 1.5f;

	level		= 1;
	exp			= 0;
	maxExp		= 100;
	moveDir		= Vec2(0, 0);
	lookDir		= Vec2(0, -1);
	isMove		= false;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Init()
{
	CImage* idleImage = LOADIMAGE(TEXT("PlayerIdle"), TEXT("Image\\PlayerIdle.bmp"));
	CImage* moveImage = LOADIMAGE(TEXT("PlayerMove"), TEXT("Image\\PlayerMove.bmp"));

	animator = new CAnimator();

	animator->CreateAnimation(TEXT("IdleUp"),			idleImage, 0.1f, 7, true, Vec2(0.f,   0.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleRightUp"),		idleImage, 0.1f, 7, true, Vec2(0.f,  70.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleRight"),		idleImage, 0.1f, 7, true, Vec2(0.f, 140.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleRightDown"),	idleImage, 0.1f, 7, true, Vec2(0.f, 210.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleDown"),			idleImage, 0.1f, 7, true, Vec2(0.f, 280.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleLeftDown"),		idleImage, 0.1f, 7, true, Vec2(0.f, 350.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleLeft"),			idleImage, 0.1f, 7, true, Vec2(0.f, 420.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));
	animator->CreateAnimation(TEXT("IdleLeftUp"),		idleImage, 0.1f, 7, true, Vec2(0.f, 490.f), Vec2(80.f, 70.f), Vec2(80.f, 0.f));

	animator->CreateAnimation(TEXT("MoveUp"),			moveImage, 0.05f, 16, true, Vec2(0.f,   0.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveRightUp"),		moveImage, 0.05f, 16, true, Vec2(0.f,  79.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveRight"),		moveImage, 0.05f, 16, true, Vec2(0.f, 158.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveRightDown"),	moveImage, 0.05f, 16, true, Vec2(0.f, 237.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveDown"),			moveImage, 0.05f, 16, true, Vec2(0.f, 316.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveLeftDown"),		moveImage, 0.05f, 16, true, Vec2(0.f, 395.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveLeft"),			moveImage, 0.05f, 16, true, Vec2(0.f, 474.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));
	animator->CreateAnimation(TEXT("MoveLeftUp"),		moveImage, 0.05f, 16, true, Vec2(0.f, 553.f), Vec2(80.f, 75.f), Vec2(84.f, 0.f));

	AddChild(animator);

	// 충돌 컴포넌트 추가
	CCollider* collider = new CCollider();
	collider->SetScale(Vec2(70, 70));
	collider->SetLayer(Layer::Player);
	AddChild(collider);
}

void CPlayer::OnEnable()
{
}

void CPlayer::Update()
{
#pragma region 이동 입력 처리

	isMove = false;

	// 이동
	if (INPUT->ButtonStay('A'))
	{
		pos.x -= speed * DT;
		isMove = true;
		moveDir.x = -1;
	}
	else if (INPUT->ButtonStay('D'))
	{
		pos.x += speed * DT;
		isMove = true;
		moveDir.x = +1;
	}
	else
	{
		moveDir.x = 0;
	}

	if (INPUT->ButtonStay('W'))
	{
		pos.y -= speed * DT;
		isMove = true;
		moveDir.y = +1;
	}
	else if (INPUT->ButtonStay('S'))
	{
		pos.y += speed * DT;
		isMove = true;
		moveDir.y = -1;
	}
	else
	{
		moveDir.y = 0;
	}

#pragma endregion


	// 경험치 및 레벨업 테스트
	if (INPUT->ButtonDown(VK_SPACE))
	{
		exp += 30;
		if (exp >= maxExp)
		{
			exp = exp - maxExp;
			level++;
			maxExp = static_cast<int>(maxExp * 1.2f);
		}
	}

	AnimatorUpdate();
}

void CPlayer::Render()
{
	// 경험치 바 렌더링 (노란 사각형)
	float progress = ((float)exp / (float)maxExp); // 0→1
	float barWidth = CGame::WINSIZE.x;
	float barHeight = 20.f;
	float offsetY = scale.y * 0.5f + 25.f; // 머리 위 여백 간격
	float barX = 0.f;
	float barY = 0.f;

	// 경험치 배경 바 렌더링 (검은 사각형)
	RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
	RENDER->SetBrush(BrushType::Solid, RGB(255, 255, 255));
	RENDER->Rect(barX, barY, barX + barWidth, barY + barHeight);

	// 최소 폭 보호
	float fillW = barWidth * progress;
	if (fillW < 2.f && progress > 0.f) fillW = 2.f;

	// 경험치 진행 바
	COLORREF fillColor = RGB(255, 255, 0);

	RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
	RENDER->SetBrush(BrushType::Solid, fillColor);
	RENDER->Rect(barX + 1.f, barY + 1.f, barX + fillW - 1.f, barY + barHeight - 1.f);

	// 현재 레벨 텍스트
	int textSize = 24;
	wstring levelText = L"Level " + to_wstring(level);
	RENDER->SetText(textSize, RGB(0, 0, 0), TextAlign::Center);
	RENDER->SetTextBackMode(TextBackMode::Null);
	RENDER->Text(
		barWidth * 0.5f,
		barHeight + 5.f,
		levelText);
}

void CPlayer::OnDisable()
{
}

void CPlayer::Release()
{
}

void CPlayer::AddExp(int amount)
{
	exp = exp + amount;
	if (exp >= maxExp)
	{
		exp = exp - maxExp;
		level++;
		maxExp = static_cast<int>(maxExp * 1.2f);
	}
}

void CPlayer::AnimatorUpdate()
{
	if (moveDir.Length() > 0)
		lookDir = moveDir;

	wstring str = TEXT("");

	if (isMove)	str += TEXT("Move");
	else			str += TEXT("Idle");

	if (lookDir.x > 0) str += TEXT("Right");
	else if (lookDir.x < 0) str += TEXT("Left");

	if (lookDir.y > 0) str += TEXT("Up");
	else if (lookDir.y < 0) str += TEXT("Down");

	animator->Play(str, false);
}
