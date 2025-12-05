#pragma once
#include "CMonster.h"
class CPlayer;

using namespace std;

enum class WeaponChoice {
    Pistol = 0,
    Shotgun = 1,
    SMG = 2
};

class CSceneStage01 : public CScene
{
public:
    CSceneStage01();
    virtual ~CSceneStage01();

public:
    void Init()     override;
    void Enter()    override;
    void Update()   override;
    void Render()   override;
    void Exit()     override;
    void Release()  override;

public:
    const std::vector<CMonster*>& GetEnemies() const { return enemies; }
    void RegisterMonster(CMonster* m);
    void UnregisterMonster(CMonster* m);
    CMonster* GetNearestEnemy(const Vec2& from, float maxRange) const;

    static void SetChosenWeapon(WeaponChoice c) { sChosenWeapon = c; };
    static WeaponChoice GetChosenWeapon() { return sChosenWeapon; };


	float playTime = 0.f;          // 플레이 시간(초)

private:
    void SpawnMonster();
    void SpawnEliteWingedMonster();
	void SpawnBossMonster();
    Vec2 GetSpawnPosPlayerDistance() const;

private:
    static WeaponChoice sChosenWeapon;

    vector<CMonster*> enemies;
    CPlayer* player = nullptr;           // 플레이어 참조

    float    spawnInterval = 3.f;        // 기본 스폰 주기(초)
    float    spawnTimer    = 0.f;        // 타이머
	int      currentMonsterCount = 0;    // 현재 씬 몬스터 수
    int      maxMonsters   = 50;         // 씬 최대 몬스터
    float    offScreenMargin = 80.f;     // 화면 밖으로 얼마나 떨어뜨릴지
    int      eliteWingSpawned = 0;              // 엘리트 1회 소환 여부
    float    eliteWingSpawnTriggerTime = 60.f;  // 소환 트리거 시간(초)
	int      bossSpawned = 0;                   // 보스 소환 여부
	float    bossSpawnTriggerTime = 3.f;        // 보스 소환 트리거 시간(초)
};

