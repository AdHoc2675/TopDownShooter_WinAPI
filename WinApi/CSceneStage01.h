#pragma once
#include "CMonster.h"
class CPlayer;

using namespace std;

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

private:
    void SpawnMonster();                 // 몬스터 1마리 스폰
    Vec2 GetSpawnPosPlayerDistance() const;

private:
    vector<CMonster*> enemies;
    CPlayer* player = nullptr;           // 플레이어 참조
    float    spawnInterval = 3.f;        // 기본 스폰 주기(초)
    float    spawnTimer    = 0.f;        // 타이머
	int      currentMonsterCount = 0;    // 현재 씬 몬스터 수
    int      maxMonsters   = 15;         // 씬 최대 몬스터
    float    offScreenMargin = 80.f;     // 화면 밖으로 얼마나 떨어뜨릴지
};

