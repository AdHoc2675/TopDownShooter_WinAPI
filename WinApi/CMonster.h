#pragma once
#include "CCombatSystem.h"

class CPlayer;
class CCollider;

class CMonster : public CGameObject
{
public:
    CMonster();
    virtual ~CMonster();
    void SetPlayer(CPlayer* p) { player = p; }
    CombatStats& GetCombatStats() { return stats; }

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;

    void OnCollisionEnter(CCollider* other) override;
    void OnCollisionStay(CCollider* other)  override;
    void OnCollisionExit(CCollider* other)  override;

    void DropExpOrb();

private:
    CPlayer*    player = nullptr;
    CCollider*  collider = nullptr;   // Ãß°¡
    CombatStats stats;
    float       hitMsgDuration;
    float       curHitMsgTime;
    wstring     hitMsg;
    bool        droppedExpOrb = false;
};

