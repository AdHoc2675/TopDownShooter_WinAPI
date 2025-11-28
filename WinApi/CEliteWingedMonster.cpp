#include "pch.h"
#include "CEliteWingedMonster.h"
#include "CPlayer.h"
#include "CMissile.h"
#include "CCombatSystem.h"

CEliteWingedMonster::CEliteWingedMonster()
{
	name  = TEXT("엘리트 날개 몬스터");
	scale = Vec2(50, 50);

	// 기본 전투 수치 조정 (GetCombatStats() 통해 설정)
	CombatStats& st = GetCombatStats();
	st.hp             = 300.f;
	st.maxHp          = 300.f;
	st.attack         = 2.f;
	st.defense        = 0.f;
	st.critChance     = 0.0f;
	st.critMultiplier = 2.0f;
	st.speed          = 120.f;   // 기본 몬스터보다 빠름
	fireInterval  = 2.0f;
	fireCooldown  = 0.f;
	missileSpeed  = 200.f;       
	attackRange   = 800.f;       
}

CEliteWingedMonster::~CEliteWingedMonster() {}

void CEliteWingedMonster::Init()
{
	collider = new CCollider();
	collider->SetScale(Vec2(55, 55));
	collider->SetLayer(Layer::Monster);
	AddChild(collider);

	animator = new CAnimator();

	// 오른쪽 이동: T_EliteWingedMonster0 (50x50 / 5프레임, 가로 배치 가정)
	CImage* moveRight = LOADIMAGE(TEXT("T_EliteWingedMonster0"), TEXT("Image\\T_EliteWingedMonster0.bmp"));
	animator->CreateAnimation(TEXT("MoveRight"), moveRight,
		0.1f, 5, true,
		Vec2(0.f, 0.f),        // 첫 프레임 시작
		Vec2(96.f, 96.f),      // 프레임 크기
		Vec2(96.f, 0.f));      // 가로 stride
	// 왼쪽 이동: T_EliteWingedMonster1 (50x50 / 5프레임, 가로 배치 가정)
	CImage* moveLeft = LOADIMAGE(TEXT("T_EliteWingedMonster1"), TEXT("Image\\T_EliteWingedMonster1.bmp"));
	animator->CreateAnimation(TEXT("MoveLeft"), moveLeft,
		0.1f, 5, true,
		Vec2(0.f, 0.f),
		Vec2(96.f, 96.f),
		Vec2(96.f, 0.f));
	AddChild(animator);
	animator->Play(TEXT("MoveRight"), true);
}

void CEliteWingedMonster::Update() {

	static float hitMsgDuration = 0.4f;
	static float curHitMsgTime = 0.f;
	if (curHitMsgTime > 0.f)
	{
		curHitMsgTime -= DT;
		if (curHitMsgTime < 0.f) curHitMsgTime = 0.f;
	}

	// 이동
	CPlayer* p = GetPlayer();
	CombatStats& st = GetCombatStats();
	Vec2 dir(0.f, 0.f);

	if (p)
	{
		dir = p->GetWorldPos() - worldPos;
		float len = dir.Length();
		if (len > 0.0001f)
		{
			dir /= len;
			pos = pos + (dir * st.speed * DT);
		}
	}

	// 이동 방향에 따라 애니메이션 선택
	if (animator)
	{
		if (dir.x < -0.01f)
			animator->Play(TEXT("MoveLeft"), false);
		else
			animator->Play(TEXT("MoveRight"), false);
	}

	// 사거리 내면 발사 시도
	if (p)
	{
		Vec2 diff = p->GetWorldPos() - worldPos;
		if (diff.SqrMagnitude() <= attackRange * attackRange)
			TryFire();
	}
}

void CEliteWingedMonster::TryFire()
{
	if (fireCooldown > 0.f)
	{
		fireCooldown -= DT;
		return;
	}

	CPlayer* p = GetPlayer();
	if (!p) return;

	Vec2 dir = p->GetWorldPos() - worldPos;
	float len = dir.Length();
	if (len < 1.f)
		return;
	dir /= len;

	SpawnMissile(dir);
	fireCooldown = fireInterval;
}

void CEliteWingedMonster::SpawnMissile(const Vec2& dir)
{
	CombatStats& st = GetCombatStats();

	CMissile* m = new CMissile();
	m->SetPos(worldPos + dir * (scale.y * 0.5f + 6.f));
	m->SetDir(dir);
	m->SetFriendly(false);
	m->SetMoveSpeed(missileSpeed);
	m->SetLifeTime(5.f);

	// 전투 수치 전달
	CombatStats& ms = m->GetCombatStats();
	ms.attack = st.attack;
	ms.defense = st.defense;
	ms.critChance = st.critChance;
	ms.critMultiplier = st.critMultiplier;

	EVENT->AddGameObject(GetScene(), m);
}

void CEliteWingedMonster::ChargeAttack()
{
}
