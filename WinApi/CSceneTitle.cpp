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
    selected = 0; // 0=Pistol, 1=Shotgun, 2=SMG
}

void CSceneTitle::Enter()
{
    CAMERA->FadeIn(0.5f);
}

void CSceneTitle::Update()
{
    // 좌/우로 선택 순환
    if (INPUT->ButtonDown(VK_LEFT, true))
    {
        selected--;
        if (selected < 0) selected = 2; // 순환
        Logger::Debug(L"[CSceneTitle] Selected: " + to_wstring(selected));
    }
    else if (INPUT->ButtonDown(VK_RIGHT, true))
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
        {
            CSceneStage01::SetChosenWeapon(WeaponChoice::Pistol);
        }
        else if (selected == 1)
        {
            CSceneStage01::SetChosenWeapon(WeaponChoice::Shotgun);
        }
        else // selected == 2
        {
            CSceneStage01::SetChosenWeapon(WeaponChoice::SMG);
        }

        WeaponChoice current = CSceneStage01::GetChosenWeapon();
        Logger::Debug(L"[CSceneTitle] Set weapon to: " + to_wstring(static_cast<int>(current)));

        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Stage01, 0.5f);
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
    RENDER->Text(cx, cy - 220.f, TEXT("무기 선택: 좌/우 화살표"));
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
                             int pierce = -1) {
        COLORREF bg = highlight ? RGB(80, 90, 120) : RGB(50, 50, 70);
        COLORREF border = highlight ? RGB(220, 220, 255) : RGB(140, 140, 180);
        
        RENDER->SetPen(PenType::Solid, border, highlight ? 3 : 2);
        RENDER->SetBrush(BrushType::Solid, bg);
        RENDER->Rect(x, y, x + boxW, y + boxH);

        float textY = y + 25.f;
        const float lineHeight = 22.f;

        // 무기 이름 (큰 글씨)
        RENDER->SetText(28, RGB(0, 0, 0), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(x + boxW * 0.5f, textY, name);
        textY += 38.f;

        // 스탯 (작은 글씨, 검은색)
        RENDER->SetText(16, RGB(0, 0, 0), TextAlign::Center);
        
        // 피해량
        wstring stat1 = L"피해량: " + to_wstring((int)damage);
        RENDER->Text(x + boxW * 0.5f, textY, stat1);
        textY += lineHeight;

        // 공격 속도 (ROF의 역수)
        float attackSpeed = (rof > 0.001f) ? (1.0f / rof) : 0.f;
        wstring stat2 = L"공격 속도: " + to_wstring((int)(attackSpeed * 10) / 10.f);
        RENDER->Text(x + boxW * 0.5f, textY, stat2);
        textY += lineHeight;

        // 투사체 수
        wstring stat3 = L"투사체 수: " + to_wstring(count);
        RENDER->Text(x + boxW * 0.5f, textY, stat3);
        textY += lineHeight;

        // 탄창 크기
        wstring stat4 = L"탄창 크기: " + to_wstring(mag);
        RENDER->Text(x + boxW * 0.5f, textY, stat4);
        textY += lineHeight;

        // 재장전 시간
        wstring stat5 = L"재장전: " + to_wstring((int)(reload * 10) / 10.f) + L"초";
        RENDER->Text(x + boxW * 0.5f, textY, stat5);
        textY += lineHeight;

        // 관통 (있을 경우만)
        if (pierce >= 0)
        {
            wstring stat6 = L"관통: " + to_wstring(pierce);
            RENDER->Text(x + boxW * 0.5f, textY, stat6);
        }
    };

    // Pistol (왼쪽)
    drawWeaponBox(startX, boxY, selected == 0, 
                  TEXT("권총"), 
                  15.f, 0.25f, 1, 6, 1.0f);

    // Shotgun (중앙)
    drawWeaponBox(startX + boxW + gap, boxY, selected == 1,
                  TEXT("샷건"),
                  10.f, 0.2f, 4, 2, 1.0f, 1);

    // SMG (오른쪽)
    drawWeaponBox(startX + (boxW + gap) * 2, boxY, selected == 2,
                  TEXT("기관단총"),
                  8.f, 0.125f, 1, 20, 2.0f);
}

void CSceneTitle::Exit()
{
}

void CSceneTitle::Release()
{
}
