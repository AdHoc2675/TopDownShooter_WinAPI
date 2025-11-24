#pragma once

class CPlayer;
class CMonster : public CGameObject
{
public:
	CMonster();
	virtual ~CMonster();
	void SetPlayer(CPlayer* player) { this->player = player; }

private:
	void Init()			override;
	void OnEnable()		override;
	void Update()		override;
	void Render()		override;
	void OnDisable()	override;
	void Release()		override;

	void OnCollisionEnter(CCollider* other) override;
	void OnCollisionStay(CCollider* other) override;
	void OnCollisionExit(CCollider* other) override;

	float speed;
	float hitMsgDuration;
	float curHitMsgTime;
	CPlayer* player = nullptr;
	wstring hitMsg;
};

