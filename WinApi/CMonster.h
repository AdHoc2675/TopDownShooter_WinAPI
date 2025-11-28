#pragma once
#include "CCombatSystem.h"

class CPlayer;
class CCollider;
class CAnimator;
class CImage;

class CMonster : public CGameObject
{
public:
    CMonster();
    virtual ~CMonster();
    void SetPlayer(CPlayer* p) { player = p; }
    CPlayer* GetPlayer() const { return player; }
    CombatStats& GetCombatStats() { return stats; }
	CAnimator* GetAnimator() const { return animator; }

protected:
    CCollider* collider = nullptr;
    CAnimator* animator = nullptr;

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
    CombatStats stats;

    float       hitMsgDuration;
    float       curHitMsgTime;
    wstring     hitMsg;
    bool        droppedExpOrb = false;
};

