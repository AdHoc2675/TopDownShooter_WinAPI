#pragma once
#include "CCombatSystem.h"

struct CombatStats;

class CPlayer;

class CMonster : public CGameObject
{
public:
    CMonster();
    virtual ~CMonster();
    void SetPlayer(CPlayer* player) { this->player = player; }

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;

    void OnCollisionEnter(CCollider* other) override;

    CPlayer*    player = nullptr;
    CombatStats stats;       
	float    	speed;
    float       hitMsgDuration;
    float       curHitMsgTime;
    wstring     hitMsg;
};

