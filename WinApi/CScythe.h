#pragma once
#include "CCombatSystem.h"

class CPlayer;
class CCollider;
class CImage;

class CScythe : public CGameObject
{
public:
    CScythe();
    virtual ~CScythe();

    void SetOwnerPlayer(CPlayer* p) { ownerPlayer = p; }
    void SetInitialAngle(float rad) { orbitAngle = rad; }
    void SetOrbit(float radius, float radPerSec) { orbitRadius = radius; angularSpeed = radPerSec; }
    CombatStats& GetCombatStats() { return stats; }

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;

    void OnCollisionEnter(CCollider* other) override;

    void UpdateOrbit();

private:
    CPlayer*    ownerPlayer = nullptr;
    CCollider*  collider    = nullptr;
    CImage*     scytheImage = nullptr;
    CombatStats stats;

    // 공전 파라미터
    float orbitRadius;
    float angularSpeed;    // 라디안/초
    float orbitAngle;
};