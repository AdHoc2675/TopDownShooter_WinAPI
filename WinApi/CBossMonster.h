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

	// 보스는 다른 몬스터와 충돌해도 밀려나지 않도록 충돌 처리 재정의
	void OnCollisionEnter(CCollider* other) override;
	void OnCollisionStay(CCollider* other) override;
	void OnCollisionExit(CCollider* other) override;
};

