#include "pch.h"
#include "CBossMonster.h"
#include "CPlayer.h"
#include "CCombatSystem.h"

CBossMonster::CBossMonster()
{
	name  = TEXT("보스 몬스터");
	scale = Vec2(180.f, 180.f);

	// 기본 전투 수치
	CombatStats& st = GetCombatStats();
	st.hp             = 2000.f;
	st.maxHp          = 2000.f;
	st.attack         = 1.f;
	st.defense        = 0.f;
	st.critChance     = 0.f;
	st.critMultiplier = 2.0f;
	st.speed          = 80.f; // 보스는 느리게 이동

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

	// 오른쪽 이동: T_EliteWingedMonster0 (112x112 / 6프레임, 가로 배치 가정)
	CImage* moveRight = LOADIMAGE(TEXT("T_HasturBoss0"), TEXT("Image\\T_HasturBoss0.bmp"));
	animator->CreateAnimation(TEXT("MoveRight"), moveRight,
		0.2f, 6, true,
		Vec2(0.f, 112.f),
		Vec2(112.f, 112.f),
		Vec2(112.f, 0.f));

	// 왼쪽 이동: T_EliteWingedMonster1 (112x112 / 6프레임, 가로 배치 가정)
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
	Vec2 dir = p->GetWorldPos() - worldPos;
	float len = dir.Length();

	if (!p) return;

	if (len > 0.0001f)
	{
		dir /= len;
		pos += dir * st.speed * DT;
	}

	if (animator)
	{
		float ax = dir.x;
		if (ax < -0.01f) animator->Play(TEXT("MoveLeft"), false);
		else             animator->Play(TEXT("MoveRight"), false);
	}
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
