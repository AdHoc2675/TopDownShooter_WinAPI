#pragma once
#include "CCombatSystem.h"

class CPlayer;

class CExpOrb : public CGameObject
{
public:
    CExpOrb();
    virtual ~CExpOrb();

    void SetPlayer(CPlayer* player) { this->player = player; }
    void SetValue(int value) { this->value = value; }

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;

    void OnCollisionEnter(CCollider* other) override;

private:
    CPlayer* player = nullptr;
    int      value  = 30;      // 획득 경험치
    float    attractRadius = 180.f;
    float    moveSpeed      = 220.f;
    float    homingSpeed    = 400.f;
    bool     isHoming       = false;
};