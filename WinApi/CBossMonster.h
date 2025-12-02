#pragma once
#include "CMonster.h"

class CBossMonster : public CMonster
{
public:
	CBossMonster();
	virtual ~CBossMonster();

private:
	void Init() override;
	void Update() override;
	void Render() override;

	// 보스는 다른 몬스터와 충돌해도 밀려나지 않도록 충돌 처리 재정의
	void OnCollisionEnter(CCollider* other) override;
	void OnCollisionStay(CCollider* other) override;
	void OnCollisionExit(CCollider* other) override;

	// 발사 패턴
	void TryFireRotatingRing();                 // 회전하는 링(스파이럴) 패턴
	void TryFireMultiRing();                    // 다중 링(겹 링) 패턴
	void SpawnMissile(const Vec2& spawnPos, const Vec2& dir);

private:
	// 공통 발사 파라미터
	float fireInterval;        // 기본 발사 간격(초)
	float fireCooldown;        // 남은 발사 쿨다운
	float missileSpeed;        // 탄 속도
	float missileLife;         // 탄 생존 시간

	// 회전 링 파라미터
	int   radialBulletCount;   // 링에서 균등 분할할 탄 수(360도)
	float phase;               // 현재 시작 각도(라디안)
	float rotationSpeed;       // 회전 속도(라디안/초) - 매 발사마다 누적

	// 다중 링 파라미터
	int   multiRingCount;      // 동시에 생성할 링 개수
	float multiRingSpacing;    // 링 간 스폰 반경 차이(픽셀)
	float multiRingSpeedScale; // 각 링마다 속도 가중치(1.0, 1.2, 1.4 ...)

	// 이동
	float chaseRange;          // 추적 범위

	// 패턴 페이즈 관리
	enum class FirePattern { RotatingRing, MultiRing } ;
	FirePattern currentPattern;
	float patternSwitchInterval; // 패턴 교체 간격(초)
	float patternTimer;
};

