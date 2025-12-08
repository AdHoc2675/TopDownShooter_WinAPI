#pragma once
#include "CMonster.h"
#include "CMissile.h"

class CEliteWingedMonster : public CMonster
{
public:
	CEliteWingedMonster();
	virtual ~CEliteWingedMonster();

private:
	void Init() override;
	void Update() override;
	void Render() override;

	void TryFire();                // 다방향 발사
	void BeginChargePrep();        // 돌진 준비(유예 시작, 위치 기억)
	void ChargeAttack();           // 유예 종료 후 실제 돌진 시작
	void UpdateCharge(float dt);   // 돌진 진행 중 이동 처리
	void SpawnMissile(const Vec2& spawnPos, const Vec2& dir);

private:
	// 이동/유지 거리
	float desiredDistance;         // 플레이어와 유지하려는 거리
	float distanceBand;            // 허용 밴드(이 안에서는 정지)

	// 발사 관련
	float fireInterval;            // 발사 간격(초)
	float fireCooldown;            // 남은 발사 쿨다운
	int   fireBulletCount;         // 동시에 발사하는 탄 수
	float fireSpreadDeg;           // 전체 확산 각도(도)
	float missileSpeed;            // 탄 속도
	float missileLife;             // 탄 생존 시간

	// 돌진 관련
	float chargeInterval;          // 돌진 간격(초)
	float chargeCooldown;          // 남은 돌진 쿨다운
	float chargePrep;			   // 유예 시간
	float chargePrepTimer;         // 현재 유예 남은 시간
	bool  chargePreparing;         // 유예 진행 중
	bool  charging;                // 실제 돌진 중
	Vec2  chargeTargetPos;         // 유예 시작 시점 플레이어 위치(기억)
	float chargeSpeed;             // 돌진 속도
	float chargeTriggerRange;     // 돌진 트리거 거리

	CSound* chargePrepSound = nullptr;
	CSound* chargeSound = nullptr;

	// 내부 상태
	Vec2  lastMoveDir;             // 애니메이션 방향 선택에 쓰기 위한 마지막 이동 방향
};

