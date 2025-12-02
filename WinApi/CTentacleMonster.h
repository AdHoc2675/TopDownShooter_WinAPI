#pragma once
#include "CMonster.h"

class CTentacleMonster : public CMonster
{
public:
	CTentacleMonster();
	virtual ~CTentacleMonster();

private:
	void Init() override;
	void Update() override;
	void OnCollisionEnter(CCollider* other) override;
	void OnCollisionStay(CCollider* other) override; // 가만히 있도록 위치 보정 무시
	void OnCollisionExit(CCollider* other) override;

private:
	// 소환 연출 상태
	bool  spawning;           // 소환 애니메이션 진행 중
	float summonTimer;        // 남은 소환 연출 시간
	float summonDuration;     // 소환 연출 총 지속시간


};

