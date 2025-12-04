#include "pch.h"
#include "CBossMonster.h"
#include "CPlayer.h"
#include "CCombatSystem.h"
#include "CGame.h"
#include "CMissile.h"
#include "CTentacleMonster.h"

CBossMonster::CBossMonster()
{
	name  = TEXT("보스 몬스터");
	scale = Vec2(112.f, 112.f);

	// 기본 전투 수치
	CombatStats& st = GetCombatStats();
	st.hp             = 7000.f;
	st.maxHp          = 7000.f;
	st.attack         = 1.f;
	st.defense        = 0.f;
	st.critChance     = 0.f;
	st.critMultiplier = 2.0f;
	st.speed          = 80.f;

	// 공통 발사/패턴 파라미터
	fireInterval = 4.0f; // 기본 발사 간격(초)
	fireCooldown = 0.f;  // 남은 발사 쿨다운
	missileSpeed = 200.f;  // 탄 속도
	missileLife = 4.0f; // 탄 생존 시간

	radialBulletCount = 12; // 링에서 균등 분할할 탄 수(360도)
	phase = 0.0f;  // 현재 시작 각도(라디안)
	rotationSpeed = 1.5f;  // 회전 속도(라디안/초) - 매 발사마다 누적

	multiRingCount = 4; // 동시에 생성할 링 개수
	multiRingSpacing = 5.f; // 링 간 스폰 반경 차이(픽셀)
	multiRingSpeedScale = 0.125f;  // 각 링마다 속도 가중치

	// 패턴/페이즈
	currentPattern = FirePattern::RotatingRing;
	bulletPatternDuration = 9.0f;   // 발사 페이즈 유지 시간
	patternTimer = bulletPatternDuration;

	chaseRange = 1200.f;

	// 촉수 소환 파라미터
	tentaclePreparing = false;
	tentaclePrepDuration = 0.75f;  // 유예 시간
	tentaclePrepTimer = 0.f;
	tentacleCount = 5;     // 소환 개수

	tentacleMinRadius = 100.f; // 플레이어 너무 근접 방지
	tentacleMaxRadius = 275.f; // 너무 멀리 퍼지지 않도록
	tentacleMinPlayerDistance = 100.f; // 안전 여유
	tentacleMinSeparation = 96.f;  // 촉수 간 최소 간격(콜라이더 직경 수준)

	ExpValue = 1000;
	ExpCount = 1;
}

CBossMonster::~CBossMonster() {}

void CBossMonster::Init()
{
	// 콜라이더
	collider = new CCollider();
	collider->SetScale(scale);
	collider->SetLayer(Layer::Monster);
	AddChild(collider);

	// 애니메이션
	animator = new CAnimator();

	CImage* moveRight = LOADIMAGE(TEXT("T_HasturBoss0"), TEXT("Image\\T_HasturBoss0.bmp"));
	animator->CreateAnimation(TEXT("MoveRight"), moveRight,
		0.2f, 6, true,
		Vec2(0.f, 112.f),
		Vec2(112.f, 112.f),
		Vec2(112.f, 0.f));

	CImage* moveLeft = LOADIMAGE(TEXT("T_HasturBoss1"), TEXT("Image\\T_HasturBoss1.bmp"));
	animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
		0.2f, 6, true,
		Vec2(0.f, 112.f),
		Vec2(112.f, 112.f),
		Vec2(112.f, 0.f));

	AddChild(animator);
	animator->Play(TEXT("MoveRight"), true);
	animator->SetRatio(1.5f);
}

void CBossMonster::Update()
{
	// 추적
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

			if (animator)
			{
				if (dir.x < -0.01f) animator->Play(TEXT("MoveLeft"), false);
				else                animator->Play(TEXT("MoveRight"), false);
			}
		}
	}

	// 회전 위상은 발사 페이즈에서만 눈에 띄도록 계속 누적
	phase += rotationSpeed * DT;
	if (phase > 2.0f * 3.141592f) phase -= 2.0f * 3.141592f;

	// 패턴 페이즈 진행
	patternTimer -= DT;

	switch (currentPattern)
	{
	case FirePattern::RotatingRing:
	{
		// 발사 페이즈: 발사만 수행
		if (fireCooldown > 0.f) fireCooldown -= DT;
		if (fireCooldown <= 0.f)
		{
			TryFireRotatingRing();
			fireCooldown = fireInterval;
		}

		if (patternTimer <= 0.f)
			SwitchToNextPattern();
	}
	break;

	case FirePattern::MultiRing:
	{
		if (fireCooldown > 0.f) fireCooldown -= DT;
		if (fireCooldown <= 0.f)
		{
			TryFireMultiRing();
			fireCooldown = fireInterval;
		}

		if (patternTimer <= 0.f)
			SwitchToNextPattern();
	}
	break;

	case FirePattern::Tentacle:
	{
		// 촉수 페이즈: 유예 표시 후 소환만 수행(발사 안 함)
		if (!tentaclePreparing)
		{
			// 페이즈 진입 직후 1회 설정
			BeginTentacleSummonPrep();
			// Tentacle 페이즈의 타이머는 유예 시간과 동일하게 맞춤
			patternTimer = tentaclePrepDuration;
		}

		// 유예 진행
		if (tentaclePreparing)
		{
			tentaclePrepTimer -= DT;
			if (tentaclePrepTimer <= 0.f)
			{
				tentaclePreparing = false;
				PerformTentacleSummon();
				// 소환 직후 다음 패턴으로 전환
				SwitchToNextPattern();
			}
		}
	}
	break;
	}
}

void CBossMonster::Render()
{
	// 체력 바
	CombatStats& st = GetCombatStats();
	float progress = st.maxHp > 0.f ? (st.hp / st.maxHp) : 0.f;
	float barWidth = CGame::WINSIZE.x * 0.7f;
	float barHeight = 30.f;
	float offsetY = scale.y * 0.5f + 70.f;
	float barX = CGame::WINSIZE.x * 0.15f;
	float barY = offsetY;

	RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
	RENDER->SetBrush(BrushType::Solid, RGB(255, 255, 255));
	RENDER->Rect(barX, barY, barX + barWidth, barY + barHeight);

	float fillW = barWidth * progress;
	if (fillW < 2.f && progress > 0.f) fillW = 2.f;

	RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
	RENDER->SetBrush(BrushType::Solid, RGB(255, 0, 0));
	RENDER->Rect(barX + 1.f, barY + 1.f, barX + fillW - 1.f, barY + barHeight - 1.f);

	// 유예 중 소환 인디케이터
	if (tentaclePreparing)
	{
		RenderSummonIndicators();
	}

	const int statSize = 24;

	// 보스 이름
	RENDER->SetText(statSize, RGB(255, 100, 100), TextAlign::Center);
	RENDER->SetTextBackMode(TextBackMode::Null);
	wstring s = L"Hastur, the Lord of Tentacles";
	RENDER->Text(CGame::WINSIZE.x * 0.5, barY - statSize, s);
}

#pragma region 발사 패턴
void CBossMonster::TryFireRotatingRing()
{
	const int count = radialBulletCount;
	if (count <= 0) return;

	const float spawnDistance = scale.y * 0.5f + 6.f;

	for (int i = 0; i < count; ++i)
	{
		const float t = (float)i / (float)count;
		const float ang = phase + t * 2.0f * 3.141592f;

		Vec2 dir(cosf(ang), sinf(ang));
		Vec2 spawnPos = worldPos + dir * spawnDistance;

		SpawnMissile(spawnPos, dir);
	}
}

void CBossMonster::TryFireMultiRing()
{
	const int count = radialBulletCount;
	if (count <= 0 || multiRingCount <= 0) return;

	const float baseRadius = scale.y * 0.5f;

	for (int r = 0; r < multiRingCount; ++r)
	{
		const float radius = baseRadius + (float)r * multiRingSpacing;
		const float speedScale = 1.0f + (float)r * multiRingSpeedScale;

		for (int i = 0; i < count; ++i)
		{
			const float t = (float)i / (float)count;
			const float ang = phase + t * 2.0f * 3.141592f + (float)r * 0.12f;

			Vec2 dir(cosf(ang), sinf(ang));
			Vec2 spawnPos = worldPos + dir * radius;

			CMissile* m = new CMissile();
			m->SetPos(spawnPos);
			m->SetDir(dir);
			m->SetFriendly(false);
			m->SetMoveSpeed(missileSpeed * speedScale);
			m->SetLifeTime(missileLife);

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

	CombatStats& st = GetCombatStats();
	CombatStats& ms = m->GetCombatStats();
	ms.attack = st.attack;
	ms.defense = st.defense;
	ms.critChance = st.critChance;
	ms.critMultiplier = st.critMultiplier;

	EVENT->AddGameObject(GetScene(), m);
}
#pragma endregion

#pragma region 촉수 소환 패턴

void CBossMonster::TryBeginTentacleSummon()
{
	if (!GetPlayer()) return;
	BeginTentacleSummonPrep();
}

void CBossMonster::BeginTentacleSummonPrep()
{
	tentaclePreparing = true;
	tentaclePrepTimer = tentaclePrepDuration;

	PlanTentaclePositions();

}

void CBossMonster::PerformTentacleSummon()
{
	for (const Vec2& pos : plannedTentaclePos)
	{
		CTentacleMonster* t = new CTentacleMonster();
		t->SetPos(pos);
		EVENT->AddGameObject(GetScene(), t);
	}

	plannedTentaclePos.clear();

}

void CBossMonster::PlanTentaclePositions()
{
	plannedTentaclePos.clear();

	CPlayer* p = GetPlayer();
	if (!p) return;

	const Vec2 center = p->GetWorldPos();
	const int  count  = tentacleCount;
	if (count <= 0) return;

	const float minR = tentacleMinRadius;
	const float maxR = tentacleMaxRadius;
	const float minPlayerDist2 = tentacleMinPlayerDistance * tentacleMinPlayerDistance;
	const float minSep2 = tentacleMinSeparation * tentacleMinSeparation;

	auto frand = []() -> float { return (float)rand() / (float)RAND_MAX; };
	auto sampleInAnnulus = [&](float rMin, float rMax) -> Vec2 {
		// 면적 균등 분포
		const float rMin2 = rMin * rMin;
		const float rMax2 = rMax * rMax;
		const float u = frand();
		const float ang = frand() * 2.0f * 3.141592f;
		const float r = sqrtf(u * (rMax2 - rMin2) + rMin2);
		return Vec2(cosf(ang) * r, sinf(ang) * r);
	};

	const int maxAttemptsPerTentacle = 24;

	for (int i = 0; i < count; ++i)
	{
		Vec2 chosen;
		bool placed = false;

		Vec2 bestCandidate;
		float bestMinDist2 = -1.f;

		for (int attempt = 0; attempt < maxAttemptsPerTentacle; ++attempt)
		{
			Vec2 offset = sampleInAnnulus(minR, maxR);
			Vec2 candidate = center + offset;

			// 플레이어와 최소 거리
			if ((candidate - center).SqrMagnitude() < minPlayerDist2)
				continue;

			// 기존 배치들과 최소 간격 검사
			float minDist2 = FLT_MAX;
			for (const Vec2& prev : plannedTentaclePos)
			{
				float d2 = (candidate - prev).SqrMagnitude();
				if (d2 < minDist2) minDist2 = d2;
			}
			if (plannedTentaclePos.empty())
				minDist2 = FLT_MAX;

			if (minDist2 >= minSep2)
			{
				chosen = candidate;
				placed = true;
				break;
			}

			if (minDist2 > bestMinDist2)
			{
				bestMinDist2 = minDist2;
				bestCandidate = candidate;
			}
		}

		if (!placed)
			chosen = bestCandidate;

		plannedTentaclePos.push_back(chosen);
	}
}

void CBossMonster::RenderSummonIndicators()
{
	const float tPassed = tentaclePrepDuration - tentaclePrepTimer;
	const float pulse = 1.0f + 0.15f * sinf(tPassed * 6.28318f * 2.0f);
	RENDER->SetPen(PenType::Solid, RGB(255, 215, 0), 2);
	RENDER->SetBrush(BrushType::Null, RGB(0, 0, 0));

	const float baseRadius = 36.f; // 촉수 콜라이더 반경 근사(72/2)
	for (const Vec2& wp : plannedTentaclePos)
	{
		// 월드 → 스크린 변환 적용
		Vec2 sp = WorldToScreen(wp);
		RENDER->Circle(sp.x, sp.y, baseRadius * pulse);
	}
}

// 월드 좌표를 현재 카메라(플레이어 중심) 기준 화면 좌표로 변환
Vec2 CBossMonster::WorldToScreen(const Vec2& w) const
{
	CPlayer* p = GetPlayer();
	if (!p) return w;

	// 플레이어의 worldPos - renderPos = 카메라 쉬프트
	Vec2 camShift = p->GetWorldPos() - p->GetRenderPos();
	return w - camShift;
}
#pragma endregion

// 패턴 순환
void CBossMonster::SwitchToNextPattern()
{
	switch (currentPattern)
	{
	case FirePattern::RotatingRing:
		currentPattern = FirePattern::MultiRing;
		patternTimer = bulletPatternDuration;
		break;
	case FirePattern::MultiRing:
		currentPattern = FirePattern::Tentacle;
		// Tentacle 페이즈는 Update 내 진입 시점에 BeginTentacleSummonPrep() 수행
		// patternTimer는 Tentacle 진입 시 유예 시간으로 재설정됨
		break;
	case FirePattern::Tentacle:
		currentPattern = FirePattern::RotatingRing;
		patternTimer = bulletPatternDuration;
		break;
	}
}

#pragma region 충돌 처리

void CBossMonster::OnCollisionEnter(CCollider* other)
{
	if (other && other->GetLayer() == Layer::Monster)
		return;

	CMonster::OnCollisionEnter(other);
}

void CBossMonster::OnCollisionStay(CCollider* other)
{
	if (other && other->GetLayer() == Layer::Monster)
		return;

	CMonster::OnCollisionStay(other);
}

void CBossMonster::OnCollisionExit(CCollider* other)
{
	CMonster::OnCollisionExit(other);
}

#pragma endregion