#pragma once
class CMonster : public CGameObject
{
public:
	CMonster();
	virtual ~CMonster();

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

	float hitMsgDuration = 0.5f;
	float curHitMsgTime = 0.f;
	wstring hitMsg = L"Hit!";
};

