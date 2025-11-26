#pragma once
#include "CCombatSystem.h"

class CPlayer;
class CImage;

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
	CImage*  CExpOrb_image = nullptr;
    int      value;      // 획득 경험치
    float    attractRadius;
    float    moveSpeed;
    float    homingSpeed;
    bool     isHoming;
};