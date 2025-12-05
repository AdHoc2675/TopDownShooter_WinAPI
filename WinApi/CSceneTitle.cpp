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
    // 기본 선택 무기
    selected = 0; // 0=Pistol, 1=Shotgun
}

void CSceneTitle::Enter()
{
    CAMERA->FadeIn(0.5f);
}

void CSceneTitle::Update()
{
    // 좌/우로 선택 변경
    if (INPUT->ButtonDown(VK_LEFT, true))
        selected = 0;
    else if (INPUT->ButtonDown(VK_RIGHT, true))
        selected = 1;

    // 위/아래도 허용(편의)
    if (INPUT->ButtonDown(VK_UP, true))
        selected = 0;
    else if (INPUT->ButtonDown(VK_DOWN, true))
        selected = 1;

    // 스페이스로 시작: 선택 결과 전달 후 씬 전환
    if (INPUT->ButtonDown(VK_SPACE, true))
    {
        Logger::Debug(L"[CSceneTitle::Update] SPACE pressed, current selected = " + to_wstring(selected));

        if (selected == 0)
        {
            Logger::Debug(L"[CSceneTitle::Update] Setting weapon to Pistol");
            CSceneStage01::SetChosenWeapon(WeaponChoice::Pistol);
        }
        else
        {
            Logger::Debug(L"[CSceneTitle::Update] Setting weapon to Shotgun");
            CSceneStage01::SetChosenWeapon(WeaponChoice::Shotgun);
        }

        // 설정 직후 값 확인
        WeaponChoice current = CSceneStage01::GetChosenWeapon();
        Logger::Debug(L"[CSceneTitle::Update] After SetChosenWeapon, GetChosenWeapon returns: " +
            to_wstring(static_cast<int>(current)));

        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Stage01, 0.5f);
    }
}

void CSceneTitle::Render()
{
    // 기본 단색 배경
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(27, 24, 29));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

    const float cx = CGame::WINSIZE.x * 0.5f;
    const float cy = CGame::WINSIZE.y * 0.5f;

    // 안내 텍스트
    RENDER->SetText(28, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(cx, cy - 160.f, TEXT("무기 선택: 좌/우 화살표"));
    RENDER->Text(cx, cy + 160.f, TEXT("스페이스로 시작"));

    // 항목 박스
    const float boxW = 320.f;
    const float boxH = 120.f;
    const float gap = 60.f;

    // Pistol 박스 (왼쪽)
    float px0 = cx - boxW - gap;
    float py0 = cy - boxH * 0.5f;
    float px1 = px0 + boxW;
    float py1 = py0 + boxH;

    // Shotgun 박스 (오른쪽)
    float sx0 = cx + gap;
    float sy0 = cy - boxH * 0.5f;
    float sx1 = sx0 + boxW;
    float sy1 = sy0 + boxH;

    // 배경 상자
    auto drawBox = [&](float x0, float y0, float x1, float y1, bool highlight) {
        COLORREF bg = highlight ? RGB(80, 90, 120) : RGB(50, 50, 70);
        COLORREF border = highlight ? RGB(220, 220, 255) : RGB(140, 140, 180);
        RENDER->SetPen(PenType::Solid, border, 2);
        RENDER->SetBrush(BrushType::Solid, bg);
        RENDER->Rect(x0, y0, x1, y1);
    };

    drawBox(px0, py0, px1, py1, selected == 0);
    drawBox(sx0, sy0, sx1, sy1, selected == 1);

    // 텍스트
    RENDER->SetText(24, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text((px0 + px1) * 0.5f, cy - 20.f, TEXT("Pistol"));
    RENDER->Text((sx0 + sx1) * 0.5f, cy - 20.f, TEXT("Shotgun"));

    // 간단 설명
    RENDER->SetText(16, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text((px0 + px1) * 0.5f, cy + 20.f, TEXT("DMG 15 | ROF 0.25 | COUNT 1 | MAG 6 | RELOAD 1.0"));
    RENDER->Text((sx0 + sx1) * 0.5f, cy + 20.f, TEXT("DMG 10 | ROF 0.2  | COUNT 4 | MAG 2 | RELOAD 1.0 | PIERCE 1"));
}

void CSceneTitle::Exit()
{
}

void CSceneTitle::Release()
{
}
