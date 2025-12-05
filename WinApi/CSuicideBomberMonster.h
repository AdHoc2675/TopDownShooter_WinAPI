#pragma once
#include "CMonster.h"

class CSuicideBomberMonster : public CMonster
{
public:
    CSuicideBomberMonster();
    virtual ~CSuicideBomberMonster();
    
protected:
    void Init() override;
    void Update() override;
    void OnCollisionEnter(CCollider* other) override;
    
private:
    void SelfDestruct(); // 자폭
    
    float detonationRange = 70.f;  // 폭발 트리거 범위
    float explosionRadius = 100.f; // 실제 폭발 반경
    float explosionDamage = 1.f;
};