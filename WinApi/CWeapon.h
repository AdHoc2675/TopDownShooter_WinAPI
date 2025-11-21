#pragma once
class CWeapon : public CGameObject
{
public:
    CWeapon();
    virtual ~CWeapon();

private:
    void    Init()          override;
    void    OnEnable()      override;
    void    Update()        override;
    void    Render()        override;
    void    OnDisable()     override;
    void    Release()       override;

private:
    void    FireToCursor();            // 마우스 지점으로 1발
    void    FireSpreadToCursor(int count, float spreadAngleDeg); // 원하는 경우 부채꼴 다발 사격
    void    CreateMissile(const Vec2& spawnPos, const Vec2& dir);

private:
    float   fireCooldown = 0.15f;      // 발사 간 최소 간격(초)
    float   curCooldown  = 0.f;
};

