#include "pch.h"
#include "CSceneStage01.h"

#include "CGame.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CRangedMonster.h"
#include "CEliteWingedMonster.h"
#include "CCameraController.h"
#include "CSoundController.h"
#include "CPanel.h"
#include "CButton.h"
#include "CWeapon.h"
#include "CCollisionManager.h"
#include "CExpOrb.h"
#include "CTiledBackground.h"
#include "CBossMonster.h"
#include "CShotgunWeapon.h"
#include "CSMGWeapon.h"
#include "CSuicideBomberMonster.h"
#include "CRocketLauncherWeapon.h"
#include "CPlayerDiamond.h"
#include "CCurrencyManager.h"

WeaponChoice CSceneStage01::sChosenWeapon = WeaponChoice::Pistol;
CharacterChoice CSceneStage01::sChosenCharacter = CharacterChoice::Shana;

CSceneStage01::CSceneStage01()
{
    Logger::Debug(L"[CSceneStage01::Constructor] Creating Stage01 scene");
    Logger::Debug(L"[CSceneStage01::Constructor] Static sChosenWeapon = " +
        to_wstring(static_cast<int>(sChosenWeapon)));


}

CSceneStage01::~CSceneStage01()
{
}

void CSceneStage01::Init()
{
 //   Logger::Debug(L"[CSceneStage01::Init] Initializing Stage01 scene");

 //   // 선택된 캐릭터에 따라 생성
 //   if (sChosenCharacter == CharacterChoice::Diamond)
 //       player = new CPlayerDiamond();
 //   else
 //       player = new CPlayer();

 //   player->SetPos(Vec2(CGame::WINSIZE.x * 0.5f, CGame::WINSIZE.y * 0.5f));
 //   AddGameObject(player);
 //   
 //   CTiledBackground* bg = new CTiledBackground();
	//bg->SetPlayer(player);
 //   AddGameObject(bg);

 //   CCameraController* controller = new CCameraController();
 //   controller->SetPlayer(player);
 //   AddGameObject(controller);

 //   CSoundController* sound = new CSoundController();
 //   AddGameObject(sound);

 //   CCollisionManager::GetInstance()->CheckLayer(Layer::Player, Layer::ExpOrb);
 //   CCollisionManager::GetInstance()->CheckLayer(Layer::Missile, Layer::Monster);
 //   CCollisionManager::GetInstance()->CheckLayer(Layer::Missile, Layer::Player);
 //   CCollisionManager::GetInstance()->CheckLayer(Layer::Player, Layer::Monster);
 //   CCollisionManager::GetInstance()->CheckLayer(Layer::Monster, Layer::Monster);
 //   
 //   spawnTimer = spawnInterval;
}

void CSceneStage01::Enter()
{
    // player가 nullptr이면 새로 생성

	if (sChosenCharacter == CharacterChoice::Diamond)
		player = new CPlayerDiamond();
	else
		player = new CPlayer();

	player->SetPos(Vec2(CGame::WINSIZE.x * 0.5f, CGame::WINSIZE.y * 0.5f));
	AddGameObject(player);

	CTiledBackground* bg = new CTiledBackground();
	bg->SetPlayer(player);
	AddGameObject(bg);

	CCameraController* controller = new CCameraController();
	controller->SetPlayer(player);
	AddGameObject(controller);

	CSoundController* sound = new CSoundController();
	AddGameObject(sound);

	CCollisionManager::GetInstance()->CheckLayer(Layer::Player, Layer::ExpOrb);
	CCollisionManager::GetInstance()->CheckLayer(Layer::Missile, Layer::Monster);
	CCollisionManager::GetInstance()->CheckLayer(Layer::Missile, Layer::Player);
	CCollisionManager::GetInstance()->CheckLayer(Layer::Player, Layer::Monster);
	CCollisionManager::GetInstance()->CheckLayer(Layer::Monster, Layer::Monster);

	spawnTimer = spawnInterval;
    

    CAMERA->FadeIn(0.5f);

    CSound* bgm = LOADSOUND(TEXT("Wasteland Combat Loop"), TEXT("Sound\\Wasteland Combat Loop.wav"));
    SOUND->PlayLoop(TEXT("Stage01_BGM"), bgm);

    WeaponChoice chosenWeapon = GetChosenWeapon();
    Logger::Debug(L"[CSceneStage01::Enter] Creating weapon: " + 
                 to_wstring(static_cast<int>(chosenWeapon)));

    if (chosenWeapon == WeaponChoice::Shotgun)
    {
        CShotgunWeapon* weapon = new CShotgunWeapon();
        player->AddChild(weapon);
        weapon->SetPlayer(player);
    }
    else if (chosenWeapon == WeaponChoice::SMG)
    {
        CSMGWeapon* weapon = new CSMGWeapon();
        player->AddChild(weapon);
        weapon->SetPlayer(player);
    }
    else if (chosenWeapon == WeaponChoice::RocketLauncher)
    {
        CRocketLauncherWeapon* weapon = new CRocketLauncherWeapon();
        player->AddChild(weapon);
        weapon->SetPlayer(player);
    }
    else // Pistol
    {
        CWeapon* weapon = new CWeapon();
        player->AddChild(weapon);
        weapon->SetPlayer(player);
    }
}

void CSceneStage01::Update()
{
    if (gameEnded)
        return;

	playTime = playTime + DT;

    if (INPUT->ButtonDown(VK_ESCAPE, true))
    {
        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Title, 1.0f);
        return;
    }
    
    // 플레이어 사망 체크
    if (player && !player->GetCombatStats().alive())
    {
        EndGame(GameResult::Defeat);
        return;
    }

    // 보스 처치 체크
    if (currentBoss && !currentBoss->GetCombatStats().alive())
    {
        EndGame(GameResult::Victory);
        return;
    }

    // 보스 생존 여부 체크
    bool bossAlive = (currentBoss != nullptr && currentBoss->GetCombatStats().alive());
    
    // 엘리트 날개 몬스터 5회 소환
    if (eliteWingSpawned < 5 && playTime >= eliteWingSpawnTriggerTime)
    {
        SpawnEliteWingedMonster();
        eliteWingSpawned++;
		eliteWingSpawnTriggerTime = eliteWingSpawnTriggerTime + 60.f; // 다음 소환 트리거 시간 갱신
    }

    if (bossSpawned == 0 && playTime >= bossSpawnTriggerTime)
    {
        SpawnBossMonster();
        bossSpawned++;
	}

    // 몬스터 지속 스폰
    spawnTimer = spawnTimer - DT;
    if (spawnTimer <= 0.f)
    {
        // 현재 몬스터 수 제한
        int monsterCount = 0;

        if (monsterCount < maxMonsters)
        {
            SpawnMonster();
        }

        // 보스가 살아있으면 스폰 간격 고정, 아니면 점점 감소
        if (bossAlive)
        {
            spawnInterval = 1.5f;  // 보스전 중에는 느린 스폰
        }
        else
        {
            // 난이도 증가: 간격 서서히 감소
            spawnInterval = spawnInterval - 0.01f;
            if (spawnInterval < 0.6f) {
                spawnInterval = 0.6f;
            }
        }
        
        spawnTimer = spawnInterval;
    }
}

void CSceneStage01::Render()
{
    Vec2 startPos = CAMERA->WorldToScreenPoint(Vec2(0, 0));
    Vec2 endPos = CAMERA->WorldToScreenPoint(Vec2(CGame::WINSIZE.x, CGame::WINSIZE.y));
}

void CSceneStage01::Exit()
{
    SOUND->Stop(TEXT("Wasteland Combat Loop"));
}

void CSceneStage01::Release()
{
}

void CSceneStage01::SpawnMonster()
{
    if (!player)
        return;

    Vec2 spawnPos = GetSpawnPosPlayerDistance();

    int r = rand() % 100;
    CMonster* monster = nullptr;
    
    if (r < 7)
    {
        monster = new CRangedMonster();
    }
    else if (r < 12)
    {
        monster = new CSuicideBomberMonster();
    }
    else
    {
        monster = new CMonster();
    }

    monster->SetPos(spawnPos);
    monster->SetPlayer(player);
    EVENT->AddGameObject(this, monster);
    RegisterMonster(monster);
}

void CSceneStage01::RegisterMonster(CMonster* m)
{
    if (m) enemies.push_back(m);
}

void CSceneStage01::UnregisterMonster(CMonster* m)
{
    if (!m) return;

    auto it = std::find(enemies.begin(), enemies.end(), m);
    if (it != enemies.end())
        enemies.erase(it);
    
    // 보스가 삭제되면 참조 해제
    if (m == currentBoss)
        currentBoss = nullptr;
}

CMonster* CSceneStage01::GetNearestEnemy(const Vec2& from, float maxRange) const
{
    CMonster* best = nullptr;
    float bestSqr = maxRange * maxRange;
    for (CMonster* m : enemies)
    {
        if (!m) continue;
        const CombatStats& st = m->GetCombatStats();
        if (!st.alive()) continue;
        Vec2 diff = m->GetWorldPos() - from;
        float sqr = diff.SqrMagnitude();
        if (sqr < bestSqr)
        {
            bestSqr = sqr;
            best = m;
        }
    }
    return best;
}

Vec2 CSceneStage01::GetSpawnPosPlayerDistance() const {
    if (!player)
        return Vec2(0, 0);

    Vec2 playerPos = player->GetWorldPos();
    Vec2 size = CGame::WINSIZE;

    int side = rand() % 4;
    Vec2 pos = playerPos;

    switch (side)
    {
    case 0: // 위쪽 화면 밖
        pos.x += (float)(rand() % (int)size.x) - size.x * 0.5f;
        pos.y -= size.y + offScreenMargin;
        break;
    case 1: // 아래쪽 화면 밖
        pos.x += (float)(rand() % (int)size.x) - size.x * 0.5f;
        pos.y += size.y + offScreenMargin;
        break;
    case 2: // 왼쪽 화면 밖
        pos.x -= size.x + offScreenMargin;
        pos.y += (float)(rand() % (int)size.y) - size.y * 0.5f;
        break;
    case 3: // 오른쪽 화면 밖
    default:
        pos.x += size.x + offScreenMargin;
        pos.y += (float)(rand() % (int)size.y) - size.y * 0.5f;
        break;
    }

    return pos;
}

void CSceneStage01::SpawnEliteWingedMonster()
{
    if (!player) return;

    Vec2 spawnPos = GetSpawnPosPlayerDistance();
    auto* elite = new CEliteWingedMonster();
    elite->SetPos(spawnPos);
    elite->SetPlayer(player);
    EVENT->AddGameObject(this, elite);
    RegisterMonster(elite);
}

void CSceneStage01::SpawnBossMonster()
{
    if (!player) return;
    Vec2 spawnPos = GetSpawnPosPlayerDistance();
    auto* boss = new CBossMonster();
    boss->SetPos(spawnPos);
    boss->SetPlayer(player);
    EVENT->AddGameObject(this, boss);
	RegisterMonster(boss);
    
    currentBoss = boss;  // 보스 참조 저장
}

void CSceneStage01::EndGame(GameResult result)
{
    if (gameEnded)
        return;

    gameEnded = true;
    SetPaused(true);

    // 보상 계산 및 지급
    int playerLevel = player ? player->GetLevel() : 1;
    int reward = CCurrencyManager::CalculateReward(monstersKilledCount, playTime, playerLevel);
    CURRENCY->AddCurrency(reward);

    Logger::Debug(L"[CSceneStage01::EndGame] Reward: " + to_wstring(reward) +
        L" (Kills:" + to_wstring(monstersKilledCount) +
        L" Time:" + to_wstring((int)playTime) +
        L" Lv:" + to_wstring(playerLevel) + L")");

    // 결과 패널 생성
    CResultPanel* resultPanel = new CResultPanel();
    EVENT->AddUI(this, resultPanel);
    resultPanel->Configure(result, playTime, playerLevel, monstersKilledCount, reward);

    Logger::Debug(L"[CSceneStage01::EndGame] Game ended: " +
        to_wstring(static_cast<int>(result)));
}