#include "pch.h"
#include "CSceneStage01.h"

#include "CGame.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CCameraController.h"
#include "CSoundController.h"
#include "CPanel.h"
#include "CButton.h"
#include "CWeapon.h"
#include "CCollisionManager.h"
#include "CExpOrb.h"

CSceneStage01::CSceneStage01()
{
}

CSceneStage01::~CSceneStage01()
{
}

void CSceneStage01::Init()
{
    player = new CPlayer();
    player->SetPos(Vec2(CGame::WINSIZE.x * 0.5f, CGame::WINSIZE.y * 0.5f));
    AddGameObject(player);

    // 처음 한마리 소환
    CMonster* monster = new CMonster();
    monster->SetPos(Vec2(CGame::WINSIZE.x * 0.5f, 100));
    monster->SetPlayer(player);
    AddGameObject(monster);

    CWeapon* weapon = new CWeapon();
    player->AddChild(weapon);
    weapon->SetPlayer(player);

    CCameraController* controller = new CCameraController();
    controller->SetPlayer(player);
    AddGameObject(controller);

    CSoundController* sound = new CSoundController();
    AddGameObject(sound);

    CPanel* panel1 = new CPanel();
    panel1->SetScale(Vec2(200, 200));
    panel1->SetPos(Vec2(100, 100));
    AddUI(panel1);

    auto click1 = [](DWORD_PTR button1, DWORD_PTR param2) {
        CButton* button = (CButton*)button1;
        wstring text = button->GetName() + TEXT("이 클릭됨");
        Logger::Debug(text);
        };

    CButton* button1 = new CButton();
    button1->SetScale(Vec2(100, 50));
    button1->SetPos(Vec2(50, 50));
    button1->SetName(TEXT("버튼1"));
    button1->SetClickCallback(click1, (DWORD_PTR)button1, 0);
    panel1->AddChild(button1);

    CPanel* panel2 = new CPanel();
    panel2->SetScale(Vec2(200, 200));
    panel2->SetPos(Vec2(300, 100));
    AddUI(panel2);

    auto click2 = [](DWORD_PTR panel1, DWORD_PTR param2) {
        CPanel* panel = (CPanel*)panel1;
        EVENT->ShowUI(panel, !panel->IsShow());
        };

    CButton* button2 = new CButton();
    button2->SetScale(Vec2(100, 50));
    button2->SetPos(Vec2(50, 50));
    button2->SetClickCallback(click2, (DWORD_PTR)panel1, 0);
    panel2->AddChild(button2);

    CCollisionManager::GetInstance()->CheckLayer(Layer::Player,  Layer::ExpOrb);
    CCollisionManager::GetInstance()->CheckLayer(Layer::Missile, Layer::Monster);
    CCollisionManager::GetInstance()->CheckLayer(Layer::Player,  Layer::Monster);
    CCollisionManager::GetInstance()->CheckLayer(Layer::Monster, Layer::Monster); // 추가

    spawnTimer = spawnInterval;
}

void CSceneStage01::Enter()
{
    CAMERA->FadeIn(0.5f);
}

void CSceneStage01::Update()
{
    if (INPUT->ButtonDown(VK_ESCAPE))
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
    Vec2 startPos = CAMERA->WorldToScreenPoint(Vec2(0, 0));
    Vec2 endPos = CAMERA->WorldToScreenPoint(Vec2(CGame::WINSIZE.x, CGame::WINSIZE.y));
}

void CSceneStage01::Exit()
{
}

void CSceneStage01::Release()
{
}

void CSceneStage01::SpawnMonster()
{
    if (!player)
        return;

    Vec2 spawnPos = GetSpawnPosPlayerDistance();

    CMonster* monster = new CMonster();
    monster->SetPos(spawnPos);
    monster->SetPlayer(player);
    EVENT->AddGameObject(this, monster);
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
