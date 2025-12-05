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
    void Render() override;
    void OnCollisionEnter(CCollider* other) override;
    
private:
    void SelfDestruct(); // 자폭
    void StartDetonationCountdown(); // 폭발 카운트다운 시작
    
    float detonationRange = 70.f;     // 폭발 트리거 범위
    float explosionRadius = 100.f;    // 실제 폭발 반경
    float explosionDamage = 1.f;
    
    bool isCountingDown = false;      // 카운트다운 중인지
    float countdownTime = 0.7ㅁ5f;       // 카운트다운 시간
    float countdownTimer = 0.f;       // 현재 카운트다운 타이머
};