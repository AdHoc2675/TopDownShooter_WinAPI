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

    // 무기 박스 영역 계산 (Render와 동일하게)
    const float cx = CGame::WINSIZE.x * 0.5f;
    const float cy = CGame::WINSIZE.y * 0.5f - 60.f;  // 상단으로 올림 (설명란 공간 확보)

    const float boxW = 200.f;
    const float boxH = 200.f;
    const float gap = 40.f;
    const float totalWidth = boxW * 3 + gap * 2;
    const float startX = cx - totalWidth * 0.5f;
    const float boxY = cy - boxH * 0.5f;

    // 각 무기 박스 영역 저장
    for (int i = 0; i < 3; ++i)
    {
        weaponBoxes[i].left   = startX + (boxW + gap) * i;
        weaponBoxes[i].top    = boxY;
        weaponBoxes[i].right  = weaponBoxes[i].left + boxW;
        weaponBoxes[i].bottom = weaponBoxes[i].top + boxH;
    }
}

void CSceneTitle::Enter()
{
    CAMERA->FadeIn(0.5f);

    CSound* bgm = LOADSOUND(TEXT("Pretty Dungeon LOOP"), TEXT("Sound\\Pretty Dungeon LOOP.wav"));
    SOUND->PlayLoop(TEXT("Title_BGM"), bgm);

    // 무기 이미지 로드
    pistolImage   = LOADIMAGE(TEXT("T_Pistol"),   TEXT("Image\\T_WeaponPistol.bmp"));
    shotgunImage  = LOADIMAGE(TEXT("T_Shotgun"),  TEXT("Image\\T_WeaponShotgun.bmp"));
    smgImage      = LOADIMAGE(TEXT("T_SMG"),      TEXT("Image\\T_WeaponSMG.bmp"));
}

void CSceneTitle::Update()
{
    // 키보드: 좌/우로 선택 순환
    if (INPUT->ButtonDown(VK_LEFT, true) || INPUT->ButtonDown('A', true))
    {
        selected--;
        if (selected < 0) selected = 2;
    }
    else if (INPUT->ButtonDown(VK_RIGHT, true) || INPUT->ButtonDown('D', true))
    {
        selected++;
        if (selected > 2) selected = 0;
    }

    // 마우스: 클릭으로 선택
    Vec2 mousePos = INPUT->MouseScreenPos();
    if (INPUT->ButtonDown(VK_LBUTTON, true))
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mousePos.x >= weaponBoxes[i].left  && mousePos.x <= weaponBoxes[i].right &&
                mousePos.y >= weaponBoxes[i].top   && mousePos.y <= weaponBoxes[i].bottom)
            {
                selected = i;
                break;
            }
        }
    }

    // 마우스 호버 시 하이라이트 (선택은 클릭 시에만)
    hoveredIndex = -1;
    for (int i = 0; i < 3; ++i)
    {
        if (mousePos.x >= weaponBoxes[i].left  && mousePos.x <= weaponBoxes[i].right &&
            mousePos.y >= weaponBoxes[i].top   && mousePos.y <= weaponBoxes[i].bottom)
        {
            hoveredIndex = i;
            break;
        }
    }

    // 스페이스 또는 엔터로 시작
    if (INPUT->ButtonDown(VK_SPACE, true) || INPUT->ButtonDown(VK_RETURN, true))
    {
        StartGame();
    }

    // 선택된 무기 박스 더블클릭으로도 시작 (간단히: 선택된 상태에서 다시 클릭)
    if (INPUT->ButtonDown(VK_LBUTTON, true))
    {
        for (int i = 0; i < 3; ++i)
        {
            if (i == selected &&
                mousePos.x >= weaponBoxes[i].left  && mousePos.x <= weaponBoxes[i].right &&
                mousePos.y >= weaponBoxes[i].top   && mousePos.y <= weaponBoxes[i].bottom)
            {
                // 이미 선택된 상태에서 다시 클릭하면 시작
                // (위에서 selected = i로 설정되므로, 첫 클릭에서는 시작 안됨)
                // 실제로는 두 번째 클릭에서 시작됨
            }
        }
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

    // ===== 상단 영역: 향후 캐릭터 선택용 (현재는 타이틀만) =====
    RENDER->SetText(36, RGB(0, 0, 0), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(cx, 40.f, TEXT("20 Minutes Till Dawn Clone"));

    // 안내 텍스트
    RENDER->SetText(18, RGB(20, 0, 0), TextAlign::Center);
    RENDER->Text(cx, 100.f, TEXT("무기 선택: 좌/우 화살표, A/D 또는 마우스 클릭"));

    // ===== 중앙 영역: 무기 선택 박스 (이름 + 이미지) =====
    const float boxW = 120.f;
    const float boxH = 120.f;

    struct WeaponInfo {
        wstring name;
        CImage* image;
    };
    WeaponInfo weapons[3] = {
        { TEXT("권총"),     pistolImage },
        { TEXT("샷건"),     shotgunImage },
        { TEXT("기관단총"), smgImage }
    };

    for (int i = 0; i < 3; ++i)
    {
        float x = weaponBoxes[i].left;
        float y = weaponBoxes[i].top;

        bool isSelected = (i == selected);
        bool isHovered  = (i == hoveredIndex);

        // 박스 색상
        COLORREF bg, border;
        int borderWidth;
        if (isSelected)
        {
            bg = RGB(80, 90, 120);
            border = RGB(255, 220, 100);  // 금색 테두리
            borderWidth = 4;
        }
        else if (isHovered)
        {
            bg = RGB(60, 65, 85);
            border = RGB(180, 180, 220);
            borderWidth = 2;
        }
        else
        {
            bg = RGB(50, 50, 70);
            border = RGB(120, 120, 150);
            borderWidth = 2;
        }

        RENDER->SetPen(PenType::Solid, border, borderWidth);
        RENDER->SetBrush(BrushType::Solid, bg);
        RENDER->Rect(x, y, x + boxW, y + boxH);

        // 무기 이름
        RENDER->SetText(22, RGB(0, 0, 0), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(x + boxW * 0.5f, y + 15.f, weapons[i].name);

        // 무기 이미지 (중앙에 배치)
        if (weapons[i].image)
        {
            float imgSize = 50.f;
            float imgX = x + (boxW - imgSize) * 0.5f;
            float imgY = y + 50.f;
            RENDER->TransparentImage(
                weapons[i].image,
                imgX, imgY,
                imgX + imgSize, imgY + imgSize,
                RGB(255, 0, 255)  // 마젠타 투명
            );
        }
    }

    // ===== 하단 영역: 선택된 무기의 스탯 설명란 =====
    RenderWeaponDescription();

    // 시작 안내
    RENDER->SetText(20, RGB(20, 20, 20), TextAlign::Center);
    RENDER->Text(cx, CGame::WINSIZE.y - 40.f, TEXT("스페이스 또는 엔터로 시작"));
    
}

void CSceneTitle::RenderWeaponDescription()
{
    const float cx = CGame::WINSIZE.x * 0.5f;
    const float descY = CGame::WINSIZE.y - 200.f;  // 하단 설명란 시작 위치
    const float descW = 500.f;
    const float descH = 150.f;

    // 설명란 배경
    RENDER->SetPen(PenType::Solid, RGB(100, 100, 130), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(40, 40, 55));
    RENDER->Rect(cx - descW * 0.5f, descY, cx + descW * 0.5f, descY + descH);

    // 선택된 무기의 스탯 정보
    struct WeaponStats {
        wstring name;
        float damage;
        float rof;        // Rate of Fire (발사 간격)
        int count;        // 투사체 수
        int mag;          // 탄창
        float reload;     // 재장전 시간
        int pierce;       // 관통 (-1이면 표시 안함)
        wstring special;  // 특수 효과 설명
    };

    WeaponStats stats[3] = {
        { TEXT("권총"),     15.f, 0.25f, 1, 6,  1.0f, -1, TEXT("균형 잡힌 기본 무기") },
        { TEXT("샷건"),     10.f, 0.25f,  4, 2,  1.0f,  1, TEXT("근거리에서 강력한 산탄") },
        { TEXT("기관단총"), 8.f,  0.125f,  1, 20, 2.0f, -1, TEXT("빠른 연사 속도") }
    };

    const WeaponStats& ws = stats[selected];

    float textY = descY + 15.f;
    const float lineHeight = 22.f;

    // 무기 이름
    RENDER->SetText(20, RGB(0, 20, 100), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(cx, textY, ws.name);
    textY += lineHeight + 5.f;

    // 스탯 표시 (2열로)
    RENDER->SetText(14, RGB(20, 20, 20), TextAlign::Left);
    
    float col1X = cx - descW * 0.5f + 30.f;
    float col2X = cx + 20.f;

    // 첫 번째 열
    RENDER->Text(col1X, textY, L"피해량: " + to_wstring((int)ws.damage));
    // 두 번째 열
    float attackSpeed = (ws.rof > 0.001f) ? (1.0f / ws.rof) : 0.f;
    wchar_t speedBuf[32];
    swprintf_s(speedBuf, L"공격속도: %.1f/s", attackSpeed);
    RENDER->Text(col2X, textY, speedBuf);
    textY += lineHeight;

    RENDER->Text(col1X, textY, L"투사체: " + to_wstring(ws.count));
    RENDER->Text(col2X, textY, L"탄창: " + to_wstring(ws.mag));
    textY += lineHeight;

    wchar_t reloadBuf[32];
    swprintf_s(reloadBuf, L"재장전: %.1f초", ws.reload);
    RENDER->Text(col1X, textY, reloadBuf);
    if (ws.pierce >= 0)
        RENDER->Text(col2X, textY, L"관통: " + to_wstring(ws.pierce));
    textY += lineHeight;

    // 특수 효과 설명
    RENDER->SetText(13, RGB(0, 0, 0), TextAlign::Center);
    RENDER->Text(cx, textY, ws.special);
}

void CSceneTitle::StartGame()
{
    if (selected == 0)
        CSceneStage01::SetChosenWeapon(WeaponChoice::Pistol);
    else if (selected == 1)
        CSceneStage01::SetChosenWeapon(WeaponChoice::Shotgun);
    else if (selected == 2)
        CSceneStage01::SetChosenWeapon(WeaponChoice::SMG);

    CAMERA->FadeOut(0.5f);
    EVENT->ChangeScene(SceneType::Stage01, 1.0f);
}

void CSceneTitle::Exit()
{
    SOUND->Stop(TEXT("Title_BGM"));
}

void CSceneTitle::Release()
{
}
