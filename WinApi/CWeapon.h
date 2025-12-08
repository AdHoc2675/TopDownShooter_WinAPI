#pragma once
#include "CSound.h"
#include "CPlayer.h"

class CWeapon : public CGameObject
{
public:
    CWeapon();
    virtual ~CWeapon();


    void ApplyUpgrade_DoubleShot_T1()
    {
        projectileCount = projectileCount + 1;
		spreadAngleDeg = spreadAngleDeg * 1.3f;   // + 30%도
        damage = damage * 0.90f;                   // -10%
        if (damage < 0.f) damage = 0.f;
    }

    void ApplyUpgrade_QuickHands_T1()
    {
        reloadTime = reloadTime * 0.80f;           // 시간 20% 감소
        fireCooldown = fireCooldown * 0.95f;       // 쿨다운 5% 감소
        if (reloadTime < 0.05f) reloadTime = 0.05f; // 과도한 감소 방지
        if (fireCooldown < 0.01f) fireCooldown = 0.01f;
    }

    void ApplyUpgrade_RapidFire_T1()
    {
        fireCooldown = fireCooldown * 0.75f;       // +25% 연사 => 쿨다운 25% 감소
        if (fireCooldown < 0.01f) fireCooldown = 0.01f;
    }

    void ApplyUpgrade_Penetration()
    {
		pierceCount += 1; // 관통 +1
		missileSpeedMultiplier *= 1.15f; // 탄환 속도 +15%
    }

    void ApplyUpgrade_ArmourPiercing() {
		damage *= 1.25f; // 피해량 +25%
		pierceCount += 2; // 관통 +2
		missileSpeedMultiplier *= 0.85f; // 탄환 속도 -25%
		maxChamberSize = maxChamberSize * 0.75f; // 탄창 크기 -25%
    }

    void ApplyUpgrade_Burn()
    {
        appliesBurn = true;
        burnStacks = 1;
        burnDuration = 5.0f;
    }

#pragma region Set/Get
    void    SetPlayer(CPlayer* player) { this->player = player; }
    void    SetFireCooldown(float cooldown) { fireCooldown = (cooldown < 0.f) ? 0.01f : cooldown; }
    void    SetMaxChamberSize(float size) { maxChamberSize = (size <= 0.f) ? 1.f : size; }
    void    SetReloadTime(float time) { reloadTime = (time < 0.f) ? 0.01f : time; }
    void    SetProjectileCount(int count) { projectileCount = (count <= 0) ? 1 : count; }
    void    SetSpreadAngleDeg(float angle) { spreadAngleDeg = (angle < 0.f) ? 0.f : angle; }
    void    SetDamage(float dmg) { damage = (dmg < 0.f) ? 0.f : dmg; }

    float   GetFireCooldown() const { return fireCooldown; }
    float   GetMaxChamberSize() const { return maxChamberSize; }
    float   GetCurChamberSize() const { return curChamberSize; }
    float   GetReloadTime() const { return reloadTime; }
    float   GetDamage() const { return damage; }  // const 추가
    int     GetProjectileCount() const { return projectileCount; }
    float   GetSpreadAngleDeg() const { return spreadAngleDeg; }
    int     GetPierceCount() const { return pierceCount; }  // 추가
    float   GetMissileSpeedMultiplier() const { return missileSpeedMultiplier; }  // 추가
    bool    GetAppliesBurn() const { return appliesBurn; }

#pragma endregion

protected:
    void    Init()          override;
    void    OnEnable()      override;
    void    Update()        override;
    void    Render()        override;
    void    OnDisable()     override;
    void    Release()       override;

    void    FireToCursor(); // 마우스 지점으로 1발
    void    FireSpreadToCursor(int count, float spreadAngleDeg); // 부채꼴 다발 사격
    void    CreateMissile(const Vec2& spawnPos, const Vec2& dir);

    float   fireCooldown;   // 발사 간 최소 간격(초)
    float   curCooldown;
	float   maxChamberSize; // 탄창 크기
	float   curChamberSize; // 현재 남은 탄창 크기
	float   reloadTime;     // 재장전 시간(초)
	float   curReloadTime;

    int     projectileCount;   // 한 번 발사 시 생성할 발사체 수
    float   spreadAngleDeg;    // 부채꼴 총 각도(도)
    float   damage;            // 무기 공격력

    int     pierceCount;           // 미사일 관통 횟수
    float   missileSpeedMultiplier; // 미사일 이동속도 배율

    bool appliesBurn = false;
    int burnStacks = 1;
    float burnDuration = 5.0f;
    float burnChance = 0.3f;  // 30% 확률

	CSound* fireSound = nullptr;
	CSound* reloadSound = nullptr;
	CPlayer* player = nullptr;
};

