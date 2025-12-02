#pragma once
#include "CMonster.h"
#include <vector>

class CBossMonster : public CMonster
{
public:
	CBossMonster();
	virtual ~CBossMonster();

private:
	void Init() override;
	void Update() override;
	void Render() override;

	// 충돌 처리: 보스는 다른 몬스터와 충돌해도 밀려나지 않음
	void OnCollisionEnter(CCollider* other) override;
	void OnCollisionStay(CCollider* other) override;
	void OnCollisionExit(CCollider* other) override;

	// 발사 패턴
	void TryFireRotatingRing();                 // 회전하는 링(스파이럴) 패턴
	void TryFireMultiRing();                    // 다중 링(겹 링) 패턴
	void SpawnMissile(const Vec2& spawnPos, const Vec2& dir);

	// 촉수 소환 패턴
	void TryBeginTentacleSummon();     // 조건 만족 시 유예 시작
	void BeginTentacleSummonPrep();    // 유예 상태 진입 및 소환 위치 계획
	void PerformTentacleSummon();      // 유예 종료 후 실제 소환
	void PlanTentaclePositions();      // 플레이어 주변 랜덤 배치
	void RenderSummonIndicators();     // 유예 중 소환 위치 시각화(링)

	Vec2 WorldToScreen(const Vec2& w) const;

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

	// 촉수 소환 파라미터(값은 Init에서 설정)
	bool  tentaclePreparing;           // 유예 중 여부
	float tentaclePrepDuration;        // 유예 총 시간(초)
	float tentaclePrepTimer;           // 유예 남은 시간
	float tentacleSummonInterval;      // 소환 쿨다운(초)
	float tentacleSummonCooldown;      // 소환 남은 쿨다운
	int   tentacleCount;               // 소환할 촉수 개수

	// 랜덤 배치 반경(도넛 영역) 및 간격
	float tentacleMinRadius;           // 플레이어로부터 최소 반경
	float tentacleMaxRadius;           // 플레이어로부터 최대 반경
	float tentacleMinPlayerDistance;   // 플레이어와의 최소 거리(안전 여유)
	float tentacleMinSeparation;       // 촉수들 간 최소 간격

	std::vector<Vec2> plannedTentaclePos;

};

