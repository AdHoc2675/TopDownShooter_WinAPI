#pragma once
#include "CSound.h"
#include "CPlayer.h"

class CWeapon : public CGameObject
{
public:
    CWeapon();
    virtual ~CWeapon();

	void SetPlayer(CPlayer* player) { this->player = player; }
    void SetProjectileCount(int count) { projectileCount = (count <= 0) ? 1 : count; }
    void SetSpreadAngleDeg(float angle) { spreadAngleDeg = (angle < 0.f) ? 0.f : angle; }
    void SetDamage(float dmg) { damage = (dmg < 0.f) ? 0.f : dmg; }

	float GetDamage() {return damage; }
    int GetProjectileCount() const { return projectileCount; }
    float GetSpreadAngleDeg() const { return spreadAngleDeg; }


private:
    void    Init()          override;
    void    OnEnable()      override;
    void    Update()        override;
    void    Render()        override;
    void    OnDisable()     override;
    void    Release()       override;

    void    FireToCursor();            // 마우스 지점으로 1발
    void    FireSpreadToCursor(int count, float spreadAngleDeg); // 원하는 경우 부채꼴 다발 사격
    void    CreateMissile(const Vec2& spawnPos, const Vec2& dir);

    float   fireCooldown;      // 발사 간 최소 간격(초)
    float   curCooldown;
	float   maxChamberSize;    // 탄창 크기
	float   curChamberSize; // 현재 남은 탄창 크기
	float   reloadTime;          // 재장전 시간(초)
	float   curReloadTime;

    int     projectileCount;   // 한 번 발사 시 생성할 발사체 수
    float   spreadAngleDeg;    // 부채꼴 총 각도(도)
    float   damage;            // 무기 공격력

	CSound* fireSound = nullptr;
	CSound* reloadSound = nullptr;
	CPlayer* player = nullptr;
};

