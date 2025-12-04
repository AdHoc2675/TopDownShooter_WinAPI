#include "pch.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CCombatSystem.h"
#include "CUpgradePanel.h"
#include "CMonster.h"
#include "CWeapon.h"

CPlayer::CPlayer()
{
	name		= TEXT("플레이어");
	scale		= Vec2(30, 50);
	animator	= nullptr;
	heartFullImage = nullptr;
	heartEmptyImage = nullptr;
	
	stats.speed		= 200.f;
	stats.hp = 5.f;
	stats.maxHp = 5.f;
	stats.defense = 0.f;
	stats.attack = 20.f;
	stats.critChance = 0.f;
	stats.critMultiplier = 1.5f;
	hitCooldown = 0.f;

	level		= 1;
	exp			= 0;
	maxExp		= 50;
	moveDir		= Vec2(0, 0);
	lookDir		= Vec2(0, -1);
	isMove		= false;

	// 발소리 초기화 (추가)
	footstepInterval = 0.35f;
	footstepTimer = 0.f;
	footstepSounds[0] = footstepSounds[1] = footstepSounds[2] = nullptr;

}

CPlayer::~CPlayer()
{
}

void CPlayer::Init()
{
	heartEmptyImage = LOADIMAGE(TEXT("HeartEmpty"), TEXT("Image\\CPlayer_hpEmpty.bmp"));
	heartFullImage = LOADIMAGE(TEXT("HeartFull"), TEXT("Image\\CPlayer_hpFull.bmp"));

	CImage* rightImage = LOADIMAGE(TEXT("PlayerMoveRight"), TEXT("Image\\T_Shana0.bmp"));
	CImage* leftImage  = LOADIMAGE(TEXT("PlayerMoveLeft"),  TEXT("Image\\T_Shana1.bmp"));

	animator = new CAnimator();

	// IdleRight / IdleLeft (7프레임, 가로 배치 가정)
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
}

void CPlayer::OnEnable()
{
}

void CPlayer::Update()
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
		AddExp(100);
	}

	AnimatorUpdate();
}

void CPlayer::Render()
{
#pragma region 경험치 바 & 레벨 렌더링
	// 경험치 바 렌더링 (노란 사각형)
	float progress = ((float)exp / (float)maxExp); // 0→1
	float barWidth = CGame::WINSIZE.x;
	float barHeight = 30.f;
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
		5.f,
		levelText);
#pragma endregion

#pragma region 체력 아이콘 렌더링
	// 체력 아이콘 렌더링
	if (heartFullImage && heartEmptyImage)
	{
		// 아이콘 크기 및 간격
		const float iconW = 48.f;
		const float iconH = 48.f;
		const float marginX = 10.f;
		const float marginY = 40.f; // 경험치 바 아래쪽 여백
		const float spacing = 6.f;

		// 좌상단 기준 위치
		float startX = 10.f;
		float startY = marginY;

		// 현재/최대 체력 정수화 및 클램프
		int curHp = (int)floorf(stats.hp);
		int maxHp = (int)floorf(stats.maxHp);
		if (curHp < 0) curHp = 0;
		if (curHp > maxHp) curHp = maxHp;
		if (maxHp < 0) maxHp = 0;

		for (int i = 0; i < maxHp; ++i)
		{
			float x0 = startX + i * (iconW + spacing);
			float y0 = startY;
			float x1 = x0 + iconW;
			float y1 = y0 + iconH;

			CImage* img = (i < curHp) ? heartFullImage : heartEmptyImage;

			RENDER->TransparentImage(
				img,
				x0, y0,    // 시작 좌표
				x1, y1,    // 끝 좌표 (width/height 아님)
				RGB(255, 0, 255)); // 마젠타 투명색
		}
	}
#pragma endregion

#pragma region 상세 스탯 정보 렌더링
	// 상세 스탯 정보 렌더링
	const int statSize = 16;
	const float startX = 10.f;
	const float startY = 40.f + 56.f; // 체력 하트 아래쪽(아이콘 높이+여백 고려)
	float y = startY;

	RENDER->SetText(statSize, RGB(20, 20, 20), TextAlign::Left);
	RENDER->SetTextBackMode(TextBackMode::Null);

	// 보기 좋게 반올림
	auto round1 = [](float v) { return (int)(v + 0.5f); };

	// ATK 표시는 플레이어 stats.attack 대신 현재 무기 damage를 우선 사용
	int atkShown = round1(stats.attack);
	int countShown = 1;
	int spreadShown = 0;
	if (weapon)
	{
		atkShown = (int)(weapon->GetDamage());
		countShown = weapon->GetProjectileCount();
		spreadShown = (int)(weapon->GetSpreadAngleDeg());
	}

	wstring s1 = L"ATK: " + to_wstring(round1(stats.attack)) +
		L"  DEF: " + to_wstring(round1(stats.defense));
	wstring s2 = L"CRIT: " + to_wstring((int)(stats.critChance * 100)) + L"%  x" +
		to_wstring(stats.critMultiplier);
	// 이동 속도 표시가 필요하면
	wstring s3 = L"SPD: " + to_wstring(round1(stats.speed));

	RENDER->Text(startX, y, s1); y += statSize + 4.f;
	RENDER->Text(startX, y, s2); y += statSize + 4.f;
	RENDER->Text(startX, y, s3);

	// 무기 요약 표시 추가
	const int weaponSize = 16;
	RENDER->SetText(weaponSize, RGB(40, 40, 40), TextAlign::Left);
	RENDER->SetTextBackMode(TextBackMode::Null);

	wstring w1 = L"Weapon DMG: " + to_wstring(atkShown);
	wstring w2 = L"COUNT: " + to_wstring(countShown);
	wstring w3 = L"SPREAD: " + to_wstring(spreadShown);

	RENDER->Text(startX, y, w1); y += weaponSize + 4.f;
	RENDER->Text(startX, y, w2); y += weaponSize + 4.f;
	RENDER->Text(startX, y, w3);

#pragma endregion
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
		maxExp = static_cast<int>(maxExp * 1.4f);

		CUpgradePanel* panel = new CUpgradePanel();
		EVENT->AddUI(GetScene(), panel);   // 씬에 먼저 추가
		panel->Configure(this);            // 구성 정보 설정
		GetScene()->SetPaused(true);
	}
}

void CPlayer::AnimatorUpdate()
{
	// 이동 중이면 현재 이동 방향을 기준으로 바라보는 방향 업데이트
	if (moveDir.Length() > 0)
		lookDir = moveDir;

	// 애니메이션 베이스(Idle/Move)
	wstring base = isMove ? TEXT("Move") : TEXT("Idle");

	// 좌우 판단: x가 0이면 직전 바라보기를 유지
	wstring dir = TEXT("");
	if (lookDir.x > 0)      dir = TEXT("Right");
	else if (lookDir.x < 0) dir = TEXT("Left");
	else                    dir = TEXT("Right"); // 기본값: Right

	// 최종 애니메이션 키
	wstring key = base + dir;
	animator->Play(key, false);
}

void CPlayer::OnCollisionEnter(CCollider* other)
{
	if (other->GetLayer() == Layer::Monster)
	{
		if (hitCooldown > 0.f) return; // 쿨다운 중이면 피해 무시

		CGameObject* orbObj = other->GetOwner();
		CMonster* monster = dynamic_cast<CMonster*>(orbObj);

		if (monster)
		{
			CombatStats& attackerStats = monster->GetCombatStats();
			COMBAT->ApplyDamage(monster, this, attackerStats, stats);

			// 1초 무적 (쿨다운 설정)
			hitCooldown = 1.0f;
		}
	}
}

void CPlayer::OnCollisionStay(CCollider* other)
{
	if (other->GetLayer() == Layer::Monster)
	{
		if (hitCooldown > 0.f) return; // 쿨다운 중이면 피해 무시

		CGameObject* obj = other->GetOwner();
		CMonster* monster = dynamic_cast<CMonster*>(obj);
		if (monster)
		{
			CombatStats& attackerStats = monster->GetCombatStats();
			COMBAT->ApplyDamage(monster, this, attackerStats, stats);

			// 1초 무적 (쿨다운 설정)
			hitCooldown = 1.0f;
		}
	}
}
