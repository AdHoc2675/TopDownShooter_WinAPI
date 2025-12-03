#pragma once
#include "CCombatSystem.h"
#include <vector>

class CPlayer;
class CImage;
class CCollider;

class CScythe : public CGameObject
{
public:
    CScythe();
    virtual ~CScythe();

    void SetOwnerPlayer(CPlayer* p) { ownerPlayer = p; }
    void SetInitialAngle(float rad) { orbitAngle = rad; }
    void SetOrbit(float radius, float radPerSec) { orbitRadius = radius; angularSpeed = radPerSec; }

	void SetAngularSpeed(float radPerSec) { angularSpeed = radPerSec; }

    CombatStats& GetCombatStats() { return stats; }
	float GetAngularSpeed() const { return angularSpeed; }

    static const std::vector<CScythe*>& GetAll() { return s_instances; }

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
    float angularSpeed;   // 라디안/초
    float orbitAngle;

    // 자전 파라미터
    float spinSpeed; // 라디안/초
    float spinAngle;

    static std::vector<CScythe*> s_instances;
};