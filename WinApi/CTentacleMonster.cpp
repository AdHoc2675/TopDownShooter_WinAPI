#include "pch.h"
#include "CTentacleMonster.h"
#include "CCombatSystem.h"

CTentacleMonster::CTentacleMonster()
{
	name  = TEXT("촉수 몬스터");
	scale = Vec2(32.f, 64.f);

	// 소환 몬스터 기본 전투 수치
	CombatStats& st = GetCombatStats();
	st.hp             = 75.f;
	st.maxHp          = 75.f;
	st.attack         = 1.f;
	st.defense        = 0.f;
	st.critChance     = 0.0f;
	st.critMultiplier = 1.0f;
	st.speed          = 0.f;     // 이동하지 않음

	// 소환몹은 경험치 드랍 없도록 설정
	ExpValue = 0;
	ExpCount = 0;
}

CTentacleMonster::~CTentacleMonster() {}

void CTentacleMonster::Init()
{
	// 콜라이더
	collider = new CCollider();
	collider->SetScale(scale);
	collider->SetLayer(Layer::Monster);
	AddChild(collider);

	animator = new CAnimator();

	CImage* idle = LOADIMAGE(TEXT("T_Tentacle"), TEXT("Image\\T_TentacleEnemy.bmp"));
	CImage* summon = LOADIMAGE(TEXT("T_Tentacle_Summon"), TEXT("Image\\T_TentacleEnemy.bmp"));

	if (summon)
	{
		animator->CreateAnimation(TEXT("Summon"), summon,
			0.2f, 3, false,          // 빠르게 재생, 루프 아님
			Vec2(0.f, 0.f),
			Vec2(64.f, 64.f),
			Vec2(64.f, 0.f));

		// 총 지속시간 계산(프레임 수 * 프레임 시간)
		summonDuration = 3 * 0.2f;
	}

	// Idle 애니메이션
	if (idle)
	{
		animator->CreateAnimation(TEXT("Idle"), idle,
			0.2f, 4, true,
			Vec2(0.f, 128.f),
			Vec2(64.f, 64.f),
			Vec2(64.f, 0.f));
	}

	AddChild(animator);

	// 소환 연출 시작
	if (summonDuration > 0.f)
	{
		spawning = true;
		summonTimer = summonDuration;
		animator->Play(TEXT("Summon"), false);
	}
}

void CTentacleMonster::Update()
{
	if (spawning)
	{
		summonTimer -= DT;
		if (summonTimer <= 0.f)
		{
			spawning = false;
			if (animator)
				animator->Play(TEXT("Idle"), true);
		}
	}

	UpdateStatusEffects();
}

void CTentacleMonster::Render()
{
	RenderStatusEffects();
}

void CTentacleMonster::OnCollisionEnter(CCollider* other)
{
	// 기본 상호작용 유지(피해 판정 등)
	CMonster::OnCollisionEnter(other);
}

void CTentacleMonster::OnCollisionStay(CCollider* other)
{
	// 가만히 있도록 위치 보정(밀림) 로직을 수행하지 않음
	// 피해/효과는 상대측(플레이어/투사체) 처리에 의해 적용될 가능성이 큼
	(void)other;
}

void CTentacleMonster::OnCollisionExit(CCollider* other)
{
	CMonster::OnCollisionExit(other);
}
