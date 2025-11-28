#include "pch.h"
#include "CSceneStage01.h"

#include "CGame.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CRangedMonster.h"
#include "CCameraController.h"
#include "CSoundController.h"
#include "CPanel.h"
#include "CButton.h"
#include "CWeapon.h"
#include "CCollisionManager.h"
#include "CExpOrb.h"
#include "CTiledBackground.h"

CSceneStage01::CSceneStage01()
{
}

CSceneStage01::~CSceneStage01()
{
}

void CSceneStage01::Init()
{

    //CTiledBackground* bg = new CTiledBackground();
    //bg->SetTileSize(32, 32);
    //bg->SetMargin(64.f);

    //// 8개 타일 로드 
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_0"), TEXT("Image\\T_ForestTile_0.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_1"), TEXT("Image\\T_ForestTile_1.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_2"), TEXT("Image\\T_ForestTile_2.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_3"), TEXT("Image\\T_ForestTile_3.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_4"), TEXT("Image\\T_ForestTile_4.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_5"), TEXT("Image\\T_ForestTile_5.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_6"), TEXT("Image\\T_ForestTile_6.bmp")));
    //bg->AddTile(LOADIMAGE(TEXT("GroundTile_7"), TEXT("Image\\T_ForestTile_7.bmp")));

    //AddGameObject(bg);

    player = new CPlayer();
    player->SetPos(Vec2(CGame::WINSIZE.x * 0.5f, CGame::WINSIZE.y * 0.5f));
    AddGameObject(player);

    auto addMonster = [&](const Vec2& pos) {
        CMonster* m = new CMonster();
        m->SetPos(pos);
        m->SetPlayer(player);
        AddGameObject(m);
        RegisterMonster(m);
    };

    addMonster(Vec2(CGame::WINSIZE.x * 0.2f, CGame::WINSIZE.y * 0.8f));
    addMonster(Vec2(CGame::WINSIZE.x * 0.4f, -CGame::WINSIZE.y * 0.8f));
    addMonster(Vec2(-CGame::WINSIZE.x * 0.9f, CGame::WINSIZE.y * 0.8f));

    CWeapon* weapon = new CWeapon();
    player->AddChild(weapon);
    weapon->SetPlayer(player);

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
}

void CSceneStage01::Enter()
{
    CAMERA->FadeIn(0.5f);

    CSound* bgm = LOADSOUND(TEXT("Wasteland Combat Loop"), TEXT("Sound\\Wasteland Combat Loop.wav"));
    SOUND->PlayLoop(TEXT("Stage01_BGM"), bgm);
}

void CSceneStage01::Update()
{
    if (INPUT->ButtonDown(VK_ESCAPE, true))
    {
        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Title, 0.5f);
        return;
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

        // 난이도 증가(선택): 간격 서서히 감소
        spawnInterval = spawnInterval - 0.02f;
        if (spawnInterval < 1.2f) {
            spawnInterval = 1.2f;
        }
        spawnTimer = spawnInterval;
    }
}

void CSceneStage01::Render()
{
	//// 배경 그리기
    //RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    //RENDER->SetBrush(BrushType::Solid, RGB(39, 32, 48));
    //RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

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

    // 5% 확률로 원거리 몬스터
    int r = rand() % 100;
    CMonster* monster = nullptr;
    if (r < 5)
    {
        monster = new CRangedMonster();
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
}

CMonster* CSceneStage01::GetNearestEnemy(const Vec2& from, float maxRange) const
{
    CMonster* best = nullptr;
    float bestSqr = maxRange * maxRange;
    for (CMonster* m : enemies)
    {
        if (!m) continue;
        const CombatStats& st = m->GetCombatStats();
        if (!st.alive()) continue; // 사망 처리된 몬스터 제외
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
