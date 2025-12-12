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
	scale		= Vec2(25, 40);
	animator	= nullptr;
	heartFullImage = nullptr;
	heartEmptyImage = nullptr;
	
	stats.speed		= 200.f;
	stats.hp = 8.f;
	stats.maxHp = 8.f;
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

	CImage* rightImage = LOADIMAGE(TEXT("PlayerRight"), TEXT("Image\\T_Shana0.bmp"));
	CImage* leftImage  = LOADIMAGE(TEXT("PlayerLeft"),  TEXT("Image\\T_Shana1.bmp"));

	animator = new CAnimator();

#pragma region 애니메이션 생성
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

#pragma endregion

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
		AddExp(maxExp / 2);
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
	const int statSize = 14;
	const float startX = 10.f;
	const float startY = 40.f + 56.f; // 체력 하트 아래쪽
	float y = startY;

	RENDER->SetText(statSize, RGB(20, 20, 20), TextAlign::Left);
	RENDER->SetTextBackMode(TextBackMode::Null);

	// 플레이어 기본 스탯
	wstring s1 = L"CRIT: " + to_wstring((int)(stats.critChance * 100)) + L"% x" +
		to_wstring((int)(stats.critMultiplier * 10) / 10.f);
	wstring s2 = L"SPD: " + to_wstring((int)(stats.speed + 0.5f));

	RENDER->Text(startX, y, s1); y += statSize + 4.f;
	RENDER->Text(startX, y, s2); y += statSize + 8.f;

	// 무기 상세 정보
	if (weapon)
	{
		RENDER->SetText(statSize, RGB(60, 60, 100), TextAlign::Left);
		
		// 무기 이름
		wstring weaponName = weapon->GetName();
		RENDER->Text(startX, y, L"[" + weaponName + L"]"); 
		y += statSize + 4.f;

		RENDER->SetText(statSize, RGB(40, 40, 40), TextAlign::Left);

		// 피해량
		int dmg = (int)(weapon->GetDamage() + 0.5f);
		wstring w1 = L"피해량: " + to_wstring(dmg);
		RENDER->Text(startX, y, w1); y += statSize + 2.f;

		// 공격 속도 (fireCooldown의 역수)
		float rof = weapon->GetFireCooldown();
		float atkSpeed = (rof > 0.001f) ? (1.0f / rof) : 0.f;
		wstring w2 = L"공격속도: " + to_wstring((int)(atkSpeed * 10) / 10.f);
		RENDER->Text(startX, y, w2); y += statSize + 2.f;

		// 투사체 수
		int count = weapon->GetProjectileCount();
		wstring w3 = L"투사체: " + to_wstring(count);
		RENDER->Text(startX, y, w3); y += statSize + 2.f;

		// 관통
		int pierce = weapon->GetPierceCount();
		wstring w4 = L"관통: " + to_wstring(pierce);
		RENDER->Text(startX, y, w4); y += statSize + 2.f;

		// 탄창 (현재/최대)
		int curMag = (int)(weapon->GetCurChamberSize() + 0.5f);
		int maxMag = (int)(weapon->GetMaxChamberSize() + 0.5f);
		wstring w5 = L"탄창: " + to_wstring(curMag) + L" / " + to_wstring(maxMag);
		RENDER->Text(startX, y, w5); y += statSize + 2.f;

		// 재장전 시간
		float reloadTime = weapon->GetReloadTime();
		wstring w6 = L"재장전: " + to_wstring((int)(reloadTime * 10) / 10.f) + L"초";
		RENDER->Text(startX, y, w6); y += statSize + 2.f;

		// 산탄각 (0이 아닐 때만 표시)
		float spread = weapon->GetSpreadAngleDeg();
		if (spread > 0.01f)
		{
			wstring w7 = L"산탄각: " + to_wstring((int)(spread + 0.5f)) + L"°";
			RENDER->Text(startX, y, w7); y += statSize + 2.f;
		}

		// 탄속 배율 (1.0이 아닐 때만 표시)
		float speedMult = weapon->GetMissileSpeedMultiplier();
		if (fabsf(speedMult - 1.0f) > 0.01f)
		{
			int speedPercent = (int)(speedMult * 100 + 0.5f);
			wstring w8 = L"탄속: " + to_wstring(speedPercent) + L"%";
			RENDER->Text(startX, y, w8);
		}
	}
	else
	{
		RENDER->Text(startX, y, L"무기 없음");
	}
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
	if (addExpSound)
		SOUND->PlayOnce(addExpSound);
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
			PlayHitSound();
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
			PlayHitSound();
			// 1초 무적 (쿨다운 설정)
			hitCooldown = 1.0f;
		}
	}
}

void CPlayer::PlayHitSound()
{
	if (hitSound)
		SOUND->PlayOnce(hitSound);
}
