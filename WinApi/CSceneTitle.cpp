#include "pch.h"
#include "CSceneTitle.h"

#include "CGame.h"
#include "CSceneStage01.h"

CSceneTitle::CSceneTitle()
{
}

CSceneTitle::~CSceneTitle()
{
}

void CSceneTitle::Init()
{
    selected = 0; // 0=Pistol, 1=Shotgun, 2=SMG, 3=RocketLauncher
}

void CSceneTitle::Enter()
{
    CAMERA->FadeIn(0.5f);

    CSound* bgm = LOADSOUND(TEXT("Pretty Dungeon LOOP"), TEXT("Sound\\Pretty Dungeon LOOP.wav"));
    SOUND->PlayLoop(TEXT("Title_BGM"), bgm);
}

void CSceneTitle::Update()
{
    // 좌/우로 선택 순환
    if (INPUT->ButtonDown(VK_LEFT, true) || INPUT->ButtonDown('A', true))
    {
        selected--;
        if (selected < 0) selected = 2; // 순환
        Logger::Debug(L"[CSceneTitle] Selected: " + to_wstring(selected));
    }
    else if (INPUT->ButtonDown(VK_RIGHT, true) || INPUT->ButtonDown('D', true))
    {
        selected++;
        if (selected > 2) selected = 0; // 순환
        Logger::Debug(L"[CSceneTitle] Selected: " + to_wstring(selected));
    }

    // 스페이스로 시작
    if (INPUT->ButtonDown(VK_SPACE, true))
    {
        Logger::Debug(L"[CSceneTitle] SPACE pressed, selected = " + to_wstring(selected));

        if (selected == 0)
            CSceneStage01::SetChosenWeapon(WeaponChoice::Pistol);
        else if (selected == 1)
            CSceneStage01::SetChosenWeapon(WeaponChoice::Shotgun);
        else if (selected == 2)
            CSceneStage01::SetChosenWeapon(WeaponChoice::SMG);
        else // selected == 3
            CSceneStage01::SetChosenWeapon(WeaponChoice::RocketLauncher);

        WeaponChoice current = CSceneStage01::GetChosenWeapon();
        Logger::Debug(L"[CSceneTitle] Set weapon to: " + to_wstring(static_cast<int>(current)));

        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Stage01, 1.0f);
    }
}

void CSceneTitle::Render()
{
    // 배경
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(27, 24, 29));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

    const float cx = CGame::WINSIZE.x * 0.5f;
    const float cy = CGame::WINSIZE.y * 0.5f;

    // 안내 텍스트
    RENDER->SetText(28, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(cx, cy - 220.f, TEXT("무기 선택: 좌/우 화살표 or A/D"));
    RENDER->Text(cx, cy + 240.f, TEXT("스페이스로 시작"));

    // 3개 무기 박스 배치
    const float boxW = 300.f;
    const float boxH = 250.f;
    const float gap = 40.f;
    const float totalWidth = boxW * 3 + gap * 2;
    const float startX = cx - totalWidth * 0.5f;
    const float boxY = cy - boxH * 0.5f;

    // 박스 그리기 헬퍼
    auto drawWeaponBox = [&](float x, float y, bool highlight, 
                             const wstring& name, 
                             float damage, float rof, int count, int mag, float reload, 
                             int pierce = -1, float explosionRadius = -1.f) {
        COLORREF bg = highlight ? RGB(80, 90, 120) : RGB(50, 50, 70);
        COLORREF border = highlight ? RGB(220, 220, 255) : RGB(140, 140, 180);
        
        RENDER->SetPen(PenType::Solid, border, highlight ? 3 : 2);
        RENDER->SetBrush(BrushType::Solid, bg);
        RENDER->Rect(x, y, x + boxW, y + boxH);

        float textY = y + 20.f;
        const float lineHeight = 20.f;

        // 무기 이름
        RENDER->SetText(24, RGB(0, 0, 0), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(x + boxW * 0.5f, textY, name);
        textY += 35.f;

        // 스탯
        RENDER->SetText(15, RGB(0, 0, 0), TextAlign::Center);
        
        wstring stat1 = L"피해량: " + to_wstring((int)damage);
        RENDER->Text(x + boxW * 0.5f, textY, stat1);
        textY += lineHeight;

        float attackSpeed = (rof > 0.001f) ? (1.0f / rof) : 0.f;
        wstring stat2 = L"공격속도: " + to_wstring((int)(attackSpeed * 10) / 10.f);
        RENDER->Text(x + boxW * 0.5f, textY, stat2);
        textY += lineHeight;

        wstring stat3 = L"투사체: " + to_wstring(count);
        RENDER->Text(x + boxW * 0.5f, textY, stat3);
        textY += lineHeight;

        wstring stat4 = L"탄창: " + to_wstring(mag);
        RENDER->Text(x + boxW * 0.5f, textY, stat4);
        textY += lineHeight;

        wstring stat5 = L"재장전: " + to_wstring((int)(reload * 10) / 10.f) + L"초";
        RENDER->Text(x + boxW * 0.5f, textY, stat5);
        textY += lineHeight;

        if (pierce >= 0)
        {
            wstring stat6 = L"관통: " + to_wstring(pierce);
            RENDER->Text(x + boxW * 0.5f, textY, stat6);
        }
        
        if (explosionRadius > 0.f)
        {
            wstring stat7 = L"폭발반경: " + to_wstring((int)explosionRadius);
            RENDER->Text(x + boxW * 0.5f, textY, stat7);
        }
    };

    // Pistol
    drawWeaponBox(startX, boxY, selected == 0, 
                  TEXT("권총"), 
                  15.f, 0.25f, 1, 6, 1.0f);

    // Shotgun
    drawWeaponBox(startX + boxW + gap, boxY, selected == 1,
                  TEXT("샷건"),
                  10.f, 0.2f, 4, 2, 1.0f, 1);

    // SMG
    drawWeaponBox(startX + (boxW + gap) * 2, boxY, selected == 2,
                  TEXT("기관단총"),
                  10.f, 0.125f, 1, 20, 2.0f);
    
    // Rocket Launcher
    //drawWeaponBox(startX + (boxW + gap) * 3, boxY, selected == 3,
    //              TEXT("로켓런처"),
    //              40.f, 1.2f, 1, 3, 2.5f, -1, 120.f);
}

void CSceneTitle::Exit()
{
    SOUND->Stop(TEXT("Title_BGM"));
}

void CSceneTitle::Release()
{
}
