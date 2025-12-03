#include "pch.h"
#include "CEliteWingedMonster.h"
#include "CPlayer.h"
#include "CMissile.h"
#include "CCombatSystem.h"

CEliteWingedMonster::CEliteWingedMonster()
{
	name  = TEXT("엘리트 날개 몬스터");
	scale = Vec2(100, 100);

	// 기본 전투 수치 조정 (GetCombatStats() 통해 설정)
	CombatStats& st = GetCombatStats();
	st.hp             = 750.f;
	st.maxHp          = 750.f;
	st.attack         = 1.f;
	st.defense        = 0.f;
	st.critChance     = 0.0f;
	st.critMultiplier = 2.0f;
	st.speed          = 120.f;   // 기본 몬스터보다 빠름

	// 거리 유지 파라미터
	desiredDistance = 260.f;
	distanceBand = 40.f;  // desired±band 사이에선 정지

	// 발사 파라미터
	fireInterval = 3.0f;
	fireCooldown = 0.f;
	fireBulletCount = 3;
	fireSpreadDeg = 850.f;
	missileSpeed = 150.f;
	missileLife = 3.0f;

	// 돌진 파라미터
	chargeInterval = 8.0f;
	chargeCooldown = 4.0f; // 시작 시 약간 대기
	chargePrep = 1.0f;  // 고정 1초
	chargePrepTimer = 0.f;
	chargePreparing = false;
	charging = false;
	chargeSpeed = 600.f;
	chargeTriggerRange = 500.f;

	lastMoveDir = Vec2(0.f, 0.f);

	ExpValue = 200;
	ExpCount = 1;
}

CEliteWingedMonster::~CEliteWingedMonster() {}

void CEliteWingedMonster::Init()
{
	collider = new CCollider();
	collider->SetScale(Vec2(100, 100));
	collider->SetLayer(Layer::Monster);
	AddChild(collider);

#pragma region 애니메이션 설정
	animator = new CAnimator();

	// 오른쪽 이동: T_EliteWingedMonster0 (96x96 / 5프레임, 가로 배치 가정)
	CImage* moveRight = LOADIMAGE(TEXT("T_EliteWingedMonster0"), TEXT("Image\\T_EliteWingedMonster0.bmp"));
	animator->CreateAnimation(TEXT("MoveRight"), moveRight,
		0.1f, 5, true,
		Vec2(0.f, 0.f),
		Vec2(96.f, 96.f),
		Vec2(96.f, 0.f));

	// 왼쪽 이동: T_EliteWingedMonster1 (96x96 / 5프레임, 가로 배치 가정)
	CImage* moveLeft = LOADIMAGE(TEXT("T_EliteWingedMonster1"), TEXT("Image\\T_EliteWingedMonster1.bmp"));
	animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
		0.1f, 5, true,
		Vec2(0.f, 0.f),
		Vec2(96.f, 96.f),
		Vec2(96.f, 0.f));

	AddChild(animator);
	animator->Play(TEXT("MoveRight"), true);
	animator->SetRatio(1.5f); // 150% 확대
#pragma endregion

	chargePrepSound = LOADSOUND(TEXT("EliteWinged_ChargePrep"), TEXT("Sound\\EliteWinged_ChargePrep.wav"));
	chargeSound = LOADSOUND(TEXT("EliteWinged_Charge"), TEXT("Sound\\EliteWinged_Charge.wav"));

}

void CEliteWingedMonster::Update()
{

	CPlayer* p = GetPlayer();
	CombatStats& st = GetCombatStats();
	Vec2 dir(0.f, 0.f);

	// 상태 머신: 돌진 준비/돌진 중 우선 처리
	if (chargePreparing)
	{
		// 유예 중(고정 1초)
		chargePrepTimer -= DT;
		if (chargePrepTimer <= 0.f)
		{
			chargePrepTimer = 0.f;
			chargePreparing = false;
			ChargeAttack(); // 실제 돌진 시작
		}
	}
	else if (charging)
	{
		UpdateCharge(DT);
	}
	else
	{
		Vec2 toPlayer = p->GetWorldPos() - worldPos;
		float dist = toPlayer.Length();
		// 평상시 이동: 유지 거리 밴드 내에서는 멈추고, 너무 가까우면 뒤로, 너무 멀면 앞으로
		if (p)
		{
			if (dist > 0.0001f)
			{
				Vec2 toDir = toPlayer / dist;
				if (dist < (desiredDistance - distanceBand))
				{
					// 너무 가까움: 반대 방향으로 물러나기
					dir = Vec2(-toDir.x, -toDir.y);
					pos += dir * st.speed * DT;
				}
				else if (dist > (desiredDistance + distanceBand))
				{
					// 너무 멀음: 가까워지기
					dir = toDir;
					pos += dir * st.speed * DT;
				}
				// 밴드 내면 정지
			}
		}

		lastMoveDir = dir;

		// 공격 트리거
		if (fireCooldown > 0.f) fireCooldown -= DT;
		if (chargeCooldown > 0.f) chargeCooldown -= DT;

		// 돌진(유예) 우선, 그 다음 사격
		if (chargeCooldown <= 0.f && dist < chargeTriggerRange )
		{
			BeginChargePrep();
			chargeCooldown = chargeInterval; // 다음 돌진까지 대기
		}
		else if (fireCooldown <= 0.f)
		{
			TryFire();
			fireCooldown = fireInterval;
		}
	}

	// 이동 방향에 따라 애니메이션 선택 (lastMoveDir 기준)
	if (animator)
	{
		float ax = lastMoveDir.x;
		if (ax < -0.01f) animator->Play(TEXT("MoveLeft"), false);
		else             animator->Play(TEXT("MoveRight"), false);
	}
}

void CEliteWingedMonster::TryFire()
{
	// 플레이어 기준 스프레드
	CPlayer* p = GetPlayer();
	if (!p) return;

	Vec2 baseDir = p->GetWorldPos() - worldPos;
	float len = baseDir.Length();
	if (len <= 0.0001f) baseDir = Vec2(0.f, -1.f);
	else baseDir /= len;

	int count = fireBulletCount;
	if (count <= 0) return;
	if (count == 1)
	{
		Vec2 spawnPos = worldPos + baseDir * (scale.y * 0.5f + 8.f);
		SpawnMissile(spawnPos, baseDir);
		return;
	}

	float half = (count - 1) * 0.5f;
	float stepDeg = (count > 1) ? fireSpreadDeg / (count - 1) : 0.f;
	float spawnDistance = scale.y * 0.5f + 8.f;

	for (int i = 0; i < count; ++i)
	{
		float offsetIndex = (float)i - half;     // -half ... +half
		float angleDeg = offsetIndex * stepDeg;
		float angleRad = angleDeg * 3.141592f / 180.f;

		Vec2 dir;
		dir.x = baseDir.x * cosf(angleRad) - baseDir.y * sinf(angleRad);
		dir.y = baseDir.x * sinf(angleRad) + baseDir.y * cosf(angleRad);

		Vec2 spawnPos = worldPos + dir * spawnDistance;
		SpawnMissile(spawnPos, dir.Normalized());
	}
}

void CEliteWingedMonster::SpawnMissile(const Vec2& spawnPos, const Vec2& dir)
{
	CMissile* m = new CMissile();
	m->SetPos(spawnPos);
	m->SetDir(dir);
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

void CEliteWingedMonster::BeginChargePrep()
{
	CPlayer* p = GetPlayer();
	if (!p) return;

	// 회피 준비 시간 1초
	chargePrepTimer = chargePrep;
	chargePreparing = true;

	// 현재 플레이어 위치 기억
	//    돌진 시작 시 현재 몬스터 위치 M과 기억 위치 P로부터
	//    목표점 = M + 2*(P - M) = 2P - M 으로 설정
	chargeTargetPos = p->GetWorldPos(); // 여기서는 '기억한 플레이어 위치'를 임시로 저장

	// 사운드 재생
	if (chargePrepSound)
	{
		SOUND->PlayOnce(chargePrepSound);
	}
}

void CEliteWingedMonster::ChargeAttack()
{
	// 돌진 시작 시점의 몬스터 위치를 기준으로 목표 재계산
	// 현재 chargeTargetPos에는 '기억한 플레이어 위치(P)'가 들어있음
	Vec2 rememberedPlayer = chargeTargetPos;
	Vec2 currentMonster   = worldPos;
	Vec2 v = rememberedPlayer - currentMonster; // M->P 벡터
	if (v.Length() <= 0.0001f)
	{
		// 같은 위치일 때 기본 방향으로 설정
		v = Vec2(0.f, -1.f);
	}
	// 목표 = M + 2*(P - M)
	Vec2 target = currentMonster + v * 2.0f;
	chargeTargetPos = target; // 실제 돌진 목표점으로 재설정

	charging = true;

	// 사운드 재생
	if (chargeSound)
	{
		SOUND->PlayOnce(chargeSound);
	}
}

void CEliteWingedMonster::UpdateCharge(float dt)
{
	// 목표점까지 직선 돌진, 도달 시 종료
	Vec2 toTarget = chargeTargetPos - worldPos;
	float len = toTarget.Length();
	if (len <= 0.0001f)
	{
		charging = false;
		return;
	}

	Vec2 dir = toTarget / len;
	lastMoveDir = dir; // 애니메이션 방향 반영

	float step = chargeSpeed * dt;
	if (step >= len)
	{
		// 목표에 도달(또는 초과)하면 정확히 목표점으로 위치 고정 후 종료
		pos = chargeTargetPos;
		charging = false;
		return;
	}

	pos += dir * step;
}

