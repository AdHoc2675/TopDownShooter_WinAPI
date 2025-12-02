#include "pch.h"
#include "CBossMonster.h"
#include "CPlayer.h"
#include "CCombatSystem.h"
#include "CGame.h"
#include "CMissile.h" // 오타 수정: 뒤의 'a' 제거

CBossMonster::CBossMonster()
{
	name  = TEXT("보스 몬스터");
	scale = Vec2(112.f, 112.f);

	// 기본 전투 수치
	CombatStats& st = GetCombatStats();
	st.hp             = 2000.f;
	st.maxHp          = 2000.f;
	st.attack         = 1.f;
	st.defense        = 0.f;
	st.critChance     = 0.f;
	st.critMultiplier = 2.0f;
	st.speed          = 80.f; // 보스는 느리게 이동

	// 공통 발사 파라미터
	fireInterval       = 3.0f;  // 더 자주 발사하여 패턴이 보이도록
	fireCooldown       = 0.f;
	missileSpeed       = 250.f;
	missileLife        = 4.0f;

	// 회전 링 파라미터
	radialBulletCount  = 18;          // 18방향
	phase              = 0.0f;        // 시작 각도
	rotationSpeed      = 0.75f;       // 라디안/초

	// 다중 링 파라미터
	multiRingCount     = 3;           // 동시 링 3개
	multiRingSpacing   = 25.f;        // 링 간 스폰 반경 차이
	multiRingSpeedScale= 0.20f;       // 각 링 속도 가중치 증가분

	// 패턴 페이즈 관리
	currentPattern       = FirePattern::RotatingRing;
	patternSwitchInterval= 8.0f;      // 8초마다 패턴 교체
	patternTimer         = patternSwitchInterval;

	chaseRange = 1200.f;

	ExpValue = 1000;
	ExpCount = 5;
}

CBossMonster::~CBossMonster() {}

void CBossMonster::Init()
{
	// 콜라이더 설정
	collider = new CCollider();
	collider->SetScale(scale);          // 보스 크기에 맞춘 히트박스
	collider->SetLayer(Layer::Monster); // 몬스터 레이어 유지
	AddChild(collider);

#pragma region 애니메이션 설정
	animator = new CAnimator();

	// 오른쪽 이동: T_HasturBoss0 (112x112 / 6프레임, 가로 배치 가정)
	CImage* moveRight = LOADIMAGE(TEXT("T_HasturBoss0"), TEXT("Image\\T_HasturBoss0.bmp"));
	animator->CreateAnimation(TEXT("MoveRight"), moveRight,
		0.2f, 6, true,
		Vec2(0.f, 112.f),
		Vec2(112.f, 112.f),
		Vec2(112.f, 0.f));

	// 왼쪽 이동: T_HasturBoss1 (112x112 / 6프레임, 가로 배치 가정)
	CImage* moveLeft = LOADIMAGE(TEXT("T_HasturBoss1"), TEXT("Image\\T_HasturBoss1.bmp"));
	animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
		0.2f, 6, true,
		Vec2(0.f, 112.f),
		Vec2(112.f, 112.f),
		Vec2(112.f, 0.f));

	AddChild(animator);
	animator->Play(TEXT("MoveRight"), true);
	animator->SetRatio(1.5f); // 150% 확대
#pragma endregion
}

void CBossMonster::Update()
{
	// 간단한 추적 이동 (기본 틀)
	CPlayer* p = GetPlayer();
	CombatStats& st = GetCombatStats();

	if (p)
	{
		Vec2 toPlayer = p->GetWorldPos() - worldPos;
		float dist = toPlayer.Length();
		if (dist < chaseRange && dist > 0.0001f)
		{
			Vec2 dir = toPlayer / dist;
			pos += dir * st.speed * DT;

			// 방향에 따른 애니메이션
			if (animator)
			{
				if (dir.x < -0.01f) animator->Play(TEXT("MoveLeft"), false);
				else                animator->Play(TEXT("MoveRight"), false);
			}
		}
	}

	// 패턴 페이즈 타이머
	patternTimer -= DT;
	if (patternTimer <= 0.f)
	{
		// 패턴 교체
		currentPattern = (currentPattern == FirePattern::RotatingRing)
			? FirePattern::MultiRing
			: FirePattern::RotatingRing;
		patternTimer = patternSwitchInterval;
	}

	// 발사 쿨다운 처리
	if (fireCooldown > 0.f) fireCooldown -= DT;

	// 회전 위상 누적: 회전 링 패턴의 연속성 유지
	phase += rotationSpeed * DT;
	if (phase > 2.0f * 3.141592f) phase -= 2.0f * 3.141592f;

	// 패턴에 따라 발사
	if (fireCooldown <= 0.f)
	{
		if (currentPattern == FirePattern::RotatingRing)
			TryFireRotatingRing();
		else
			TryFireMultiRing();

		fireCooldown = fireInterval;
	}
}

void CBossMonster::Render()
{
#pragma region 체력 바 렌더링
	// 체력 바 렌더링 (빨간 사각형)
	CombatStats& st = GetCombatStats();
	float progress = ((float)st.hp / (float)st.maxHp); // 0→1
	float barWidth = CGame::WINSIZE.x * 0.7f;
	float barHeight = 30.f;
	float offsetY = scale.y * 0.5f + 70.f; // 머리 위 여백 간격
	float barX = CGame::WINSIZE.x * 0.15f;
	float barY = offsetY;

	// 체력 배경 바 렌더링 (검은 사각형)
	RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
	RENDER->SetBrush(BrushType::Solid, RGB(255, 255, 255));
	RENDER->Rect(barX, barY, barX + barWidth, barY + barHeight);

	// 최소 폭 보호
	float fillW = barWidth * progress;
	if (fillW < 2.f && progress > 0.f) fillW = 2.f;

	// 체력 진행 바
	COLORREF fillColor = RGB(255, 0, 0);

	RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
	RENDER->SetBrush(BrushType::Solid, fillColor);
	RENDER->Rect(barX + 1.f, barY + 1.f, barX + fillW - 1.f, barY + barHeight - 1.f);

#pragma endregion
}

// 회전하는 링(스파이럴): 이전 위상(phase)을 기준으로 균등 분할 + 약간의 회전이 누적됨
void CBossMonster::TryFireRotatingRing()
{
	const int count = radialBulletCount;
	if (count <= 0) return;

	const float spawnBase = scale.y * 0.5f + 12.f;

	for (int i = 0; i < count; ++i)
	{
		// 균등 분할 각도 + 현재 위상
		const float t   = (float)i / (float)count;
		const float ang = phase + t * 2.0f * 3.141592f;

		Vec2 dir(cosf(ang), sinf(ang));
		Vec2 spawnPos = worldPos + dir * spawnBase;
		
		SpawnMissile(spawnPos, dir);
	}
}

// 다중 링(겹 링): 서로 다른 반경/속도를 가진 여러 링을 동시에 발사
void CBossMonster::TryFireMultiRing()
{
	const int count = radialBulletCount;
	if (count <= 0 || multiRingCount <= 0) return;

	const float baseRadius = scale.y * 0.5f + 12.f;

	for (int r = 0; r < multiRingCount; ++r)
	{
		const float radius = baseRadius + (float)r * multiRingSpacing;
		const float speedScale = 1.0f + (float)r * multiRingSpeedScale;

		for (int i = 0; i < count; ++i)
		{
			const float t   = (float)i / (float)count;
			// 링마다 약간의 각도 오프셋을 둬 겹침을 피함
			const float ang = phase + t * 2.0f * 3.141592f + (float)r * 0.12f;

			Vec2 dir(cosf(ang), sinf(ang));
			Vec2 spawnPos = worldPos + dir * radius;

			// 링별 속도 가중치를 반영한 미사일 생성
			CMissile* m = new CMissile();
			m->SetPos(spawnPos);
			m->SetDir(dir);
			m->SetFriendly(false);
			m->SetMoveSpeed(missileSpeed * speedScale);
			m->SetLifeTime(missileLife);

			// 공격 스탯 전달
			CombatStats& st = GetCombatStats();
			CombatStats& ms = m->GetCombatStats();
			ms.attack         = st.attack;
			ms.defense        = st.defense;
			ms.critChance     = st.critChance;
			ms.critMultiplier = st.critMultiplier;

			EVENT->AddGameObject(GetScene(), m);
		}
	}
}

void CBossMonster::SpawnMissile(const Vec2& spawnPos, const Vec2& dir)
{
	CMissile* m = new CMissile();
	m->SetPos(spawnPos);
	m->SetDir(dir.Normalized());
	m->SetFriendly(false);
	m->SetMoveSpeed(missileSpeed);
	m->SetLifeTime(missileLife);

	// 공격 스탯 전달
	CombatStats& st = GetCombatStats();
	CombatStats& ms = m->GetCombatStats();
	ms.attack = st.attack;
	ms.defense = st.defense;
	ms.critChance = st.critChance;
	ms.critMultiplier = st.critMultiplier;

	EVENT->AddGameObject(GetScene(), m);
}

void CBossMonster::OnCollisionEnter(CCollider* other)
{
	// 몬스터-몬스터 충돌에서는 보스가 밀려나지 않도록 아무 것도 하지 않음
	if (other && other->GetLayer() == Layer::Monster)
		return;

	// 그 외(플레이어/투사체 등)는 기본 동작 유지
	CMonster::OnCollisionEnter(other);
}

void CBossMonster::OnCollisionStay(CCollider* other)
{
	// 몬스터-몬스터 충돌 유지 시에도 보스는 위치 보정(밀림)을 하지 않음
	if (other && other->GetLayer() == Layer::Monster)
		return;

	CMonster::OnCollisionStay(other);
}

void CBossMonster::OnCollisionExit(CCollider* other)
{
	// 기본 동작 유지 (상태 클리어 등)
	CMonster::OnCollisionExit(other);
}
