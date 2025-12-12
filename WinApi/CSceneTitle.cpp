#include "pch.h"
#include "CSceneTitle.h"

#include "CGame.h"
#include "CSceneStage01.h"

#include "CCurrencyManager.h" // CURRENCY
#include <fstream>

CSceneTitle::CSceneTitle()
{
}

CSceneTitle::~CSceneTitle()
{
}

void CSceneTitle::Init()
{
    selectedWeapon = 0;
    selectedCharacter = 0;

    const float cx = CGame::WINSIZE.x * 0.5f;

	UIClickSound = LOADSOUND(TEXT("UI_Click"), TEXT("Sound\\UI Click 36.wav"));

    // ===== 캐릭터 박스 영역 (상단) =====
    const float charBoxW = 150.f;
    const float charBoxH = 150.f;
    const float charGap = 30.f;
    const float charTotalWidth = charBoxW * 2 + charGap;
    const float charStartX = cx - charTotalWidth * 0.5f;
    const float charBoxY = 130.f;

    for (int i = 0; i < 2; ++i)
    {
        characterBoxes[i].left   = charStartX + (charBoxW + charGap) * i;
        characterBoxes[i].top    = charBoxY;
        characterBoxes[i].right  = characterBoxes[i].left + charBoxW;
        characterBoxes[i].bottom = characterBoxes[i].top + charBoxH;
    }

    // ===== 무기 박스 영역 (중앙) =====
    const float boxW = 120.f;
    const float boxH = 120.f;
    const float gap = 30.f;
    const float totalWidth = boxW * 3 + gap * 2;
    const float startX = cx - totalWidth * 0.5f;
    const float boxY = 320.f;

    for (int i = 0; i < 3; ++i)
    {
        weaponBoxes[i].left   = startX + (boxW + gap) * i;
        weaponBoxes[i].top    = boxY;
        weaponBoxes[i].right  = weaponBoxes[i].left + boxW;
        weaponBoxes[i].bottom = weaponBoxes[i].top + boxH;
    }

    // 업그레이드 UI 박스 위치
    // 무기 박스 하단과 무기 설명 상단 사이 중앙
    const float uiW = 260.f;
    const float uiH = 80.f;
    const float uiXLeft = cx - uiW - 20.f;
    const float uiXRight = cx + 20.f;

    const float weaponsBottom = boxY + boxH;                 // 무기 박스 하단
    const float descTop = CGame::WINSIZE.y - 220.f;           // 무기 설명 상단
    
    float centerBetween = weaponsBottom + (descTop - weaponsBottom) * 0.5f;
    float uiTop = centerBetween - uiH * 0.5f;

    upgradeDummyBox.left = uiXLeft;
    upgradeDummyBox.top = uiTop;
    upgradeDummyBox.right = uiXLeft + uiW;
    upgradeDummyBox.bottom = uiTop + uiH;

    upgradeSpeedBox.left = uiXRight;
    upgradeSpeedBox.top = uiTop; 
    upgradeSpeedBox.right = uiXRight + uiW;
    upgradeSpeedBox.bottom = uiTop + uiH;
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

    // 캐릭터 이미지 로드
    shanaImage   = LOADIMAGE(TEXT("T_Shana"),   TEXT("Image\\T_Shana0.bmp"));
    diamondImage = LOADIMAGE(TEXT("T_Diamond"), TEXT("Image\\T_Diamond0.bmp"));

    // 업그레이드 상태 불러오기 (영구 저장)
    LoadTitleUpgrades();
}

void CSceneTitle::Update()
{
    Vec2 mousePos = INPUT->MouseScreenPos();

    // ===== 캐릭터 선택 (키보드: 1, 2) =====
    if (INPUT->ButtonDown('1', true)) {
        selectedCharacter = 0;
        if (UIClickSound) {
            SOUND->PlayOnce(UIClickSound);
        }
    }

    if (INPUT->ButtonDown('2', true)) {
        selectedCharacter = 1;
        if (UIClickSound) {
            SOUND->PlayOnce(UIClickSound);
        }
    }


	// ===== 무기 선택 (키보드: 좌/우) =====
	if (INPUT->ButtonDown(VK_LEFT, true) || INPUT->ButtonDown('A', true))
	{
		selectedWeapon--;
		if (selectedWeapon < 0) selectedWeapon = 2;

		if (UIClickSound) {
			SOUND->PlayOnce(UIClickSound);
		}
	}
	else if (INPUT->ButtonDown(VK_RIGHT, true) || INPUT->ButtonDown('D', true))
	{
		selectedWeapon++;
		if (selectedWeapon > 2) selectedWeapon = 0;

		if (UIClickSound) {
			SOUND->PlayOnce(UIClickSound);
		}
	}

	// ===== 마우스 클릭으로 선택 =====
	if (INPUT->ButtonDown(VK_LBUTTON, true))
	{
		// 캐릭터 박스 클릭
		for (int i = 0; i < 2; ++i)
		{
			if (mousePos.x >= characterBoxes[i].left && mousePos.x <= characterBoxes[i].right &&
				mousePos.y >= characterBoxes[i].top && mousePos.y <= characterBoxes[i].bottom)
			{
				selectedCharacter = i;
				if (UIClickSound) {
					SOUND->PlayOnce(UIClickSound);
				}

				break;
			}
		}

		// 무기 박스 클릭
		for (int i = 0; i < 3; ++i)
		{
			if (mousePos.x >= weaponBoxes[i].left && mousePos.x <= weaponBoxes[i].right &&
				mousePos.y >= weaponBoxes[i].top && mousePos.y <= weaponBoxes[i].bottom)
			{
				selectedWeapon = i;
				if (UIClickSound) {
					SOUND->PlayOnce(UIClickSound);
				}

				break;
			}
		}
	}

	// ===== 마우스 호버 =====
	hoveredCharacterIndex = -1;
	for (int i = 0; i < 2; ++i)
	{
		if (mousePos.x >= characterBoxes[i].left && mousePos.x <= characterBoxes[i].right &&
			mousePos.y >= characterBoxes[i].top && mousePos.y <= characterBoxes[i].bottom)
		{
			hoveredCharacterIndex = i;
			break;
		}
	}

	hoveredWeaponIndex = -1;
	for (int i = 0; i < 3; ++i)
	{
		if (mousePos.x >= weaponBoxes[i].left && mousePos.x <= weaponBoxes[i].right &&
			mousePos.y >= weaponBoxes[i].top && mousePos.y <= weaponBoxes[i].bottom)
		{
			hoveredWeaponIndex = i;
			break;
		}
	}

    // ===== 업그레이드 UI 입력 처리 (아이콘 1초 홀드) =====
    bool mouseInDummy = (mousePos.x >= upgradeDummyBox.left && mousePos.x <= upgradeDummyBox.right &&
                         mousePos.y >= upgradeDummyBox.top  && mousePos.y <= upgradeDummyBox.bottom);

    bool mouseInSpeed = (mousePos.x >= upgradeSpeedBox.left && mousePos.x <= upgradeSpeedBox.right &&
                         mousePos.y >= upgradeSpeedBox.top  && mousePos.y <= upgradeSpeedBox.bottom);

    // 시작 홀드
    if (INPUT->ButtonDown(VK_LBUTTON, true))
    {
        purchasedThisHold = false;
        if (mouseInDummy) {
            isHolding = true;
            holdingIndex = 0;
            holdTimer = 0.f;
        }
        else if (mouseInSpeed) {
            isHolding = true;
            holdingIndex = 1;
            holdTimer = 0.f;
        }
        else {
            // 클릭 외부: 취소
            isHolding = false;
            holdingIndex = -1;
            holdTimer = 0.f;
        }
    }

    // 홀드 중
    if (isHolding && INPUT->ButtonStay(VK_LBUTTON))
    {
        // 마우스가 박스 안에 유지되는지 확인 (같은 박스)
        bool stillOver = (holdingIndex == 0) ? mouseInDummy : mouseInSpeed;
        if (!stillOver) {
            // 마우스가 벗어나면 취소
            isHolding = false;
            holdingIndex = -1;
            holdTimer = 0.f;
        } else {
            holdTimer += DT;
            // 1초 경과 시 구매 시도 (한 번만)
            if (holdTimer >= 1.0f && !purchasedThisHold)
            {
                if (holdingIndex == 0)
                {
                    // dummy upgrade (무한, 비용 500)
                    if (CURRENCY->CanAfford(kDummyCost))
                    {
                        CURRENCY->SpendCurrency(kDummyCost);
                        dummyUpgradeCount++;
                        if (UIClickSound) SOUND->PlayOnce(UIClickSound);
                        SaveTitleUpgrades();
                    }
                }
                else if (holdingIndex == 1)
                {
                    // speed upgrade (max 3, 비용 계단)
                    if (speedUpgradeLevel < 3)
                    {
                        int cost = speedCosts[speedUpgradeLevel];
                        if (CURRENCY->CanAfford(cost))
                        {
                            CURRENCY->SpendCurrency(cost);
                            speedUpgradeLevel++;
                            if (UIClickSound) SOUND->PlayOnce(UIClickSound);
                            SaveTitleUpgrades();
                        }
                    }
                }

                purchasedThisHold = true;
                // require release before another purchase
            }
        }
    }

    // 마우스 버튼 업이면 홀드 상태 초기화
    if (INPUT->ButtonUp(VK_LBUTTON, true))
    {
        isHolding = false;
        holdingIndex = -1;
        holdTimer = 0.f;
        purchasedThisHold = false;
    }

	// ===== 게임 시작 =====
	if (INPUT->ButtonDown(VK_SPACE, true) || INPUT->ButtonDown(VK_RETURN, true))
	{
		StartGame();
	}
}


void CSceneTitle::Render()
{
    // 배경
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(27, 24, 29));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

    const float cx = CGame::WINSIZE.x * 0.5f;

    // ===== 타이틀 =====
    RENDER->SetText(36, RGB(20, 20, 20), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(cx, 30.f, TEXT("20 Minutes Till Dawn Clone"));

    // ===== 캐릭터 선택 안내 =====
    RENDER->SetText(18, RGB(30, 30, 30), TextAlign::Center);
    RENDER->Text(cx, 100.f, TEXT("캐릭터 선택 (1, 2 또는 클릭)"));

    // ===== 캐릭터 박스 렌더링 =====
    const float charBoxW = 150.f;
    const float charBoxH = 150.f;

    struct CharacterInfo {
        wstring name;
        CImage* image;
    };

    CharacterInfo characters[2] = {
        { TEXT("Shana"),   shanaImage },
        { TEXT("Diamond"), diamondImage }
    };

    for (int i = 0; i < 2; ++i)
    {
        float x = characterBoxes[i].left;
        float y = characterBoxes[i].top;

        bool isSelected = (i == selectedCharacter);
        bool isHovered  = (i == hoveredCharacterIndex);

        COLORREF bg, border;
        int borderWidth;
        if (isSelected)
        {
            bg = RGB(100, 80, 120);
            border = RGB(255, 180, 100);
            borderWidth = 4;
        }
        else if (isHovered)
        {
            bg = RGB(70, 60, 90);
            border = RGB(180, 180, 220);
            borderWidth = 2;
        }
        else
        {
            bg = RGB(50, 45, 65);
            border = RGB(120, 120, 150);
            borderWidth = 2;
        }

        RENDER->SetPen(PenType::Solid, border, borderWidth);
        RENDER->SetBrush(BrushType::Solid, bg);
        RENDER->Rect(x, y, x + charBoxW, y + charBoxH);

        // 캐릭터 이름
        RENDER->SetText(18, RGB(20, 20, 20), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(x + charBoxW * 0.5f, y + 10.f, characters[i].name);

        // 캐릭터 이미지 (FrameImage로 첫 프레임만)
        if (characters[i].image)
        {
            float imgSize = 64.f;
            float imgX = x + (charBoxW - imgSize) * 0.5f;
            float imgY = y + 40.f;
            // 첫 프레임만 표시 (32x32 프레임 기준)
            RENDER->FrameImage(
                characters[i].image,
                imgX, imgY, imgX + imgSize, imgY + imgSize,
                0.f, 0.f, 32.f, 32.f,
                RGB(255, 0, 255)
            );
        }
    }

    // ===== 무기 선택 안내 =====
    RENDER->SetText(18, RGB(30, 30, 30), TextAlign::Center);
    RENDER->Text(cx, 290.f, TEXT("무기 선택 (좌/우 화살표, A/D 또는 클릭)"));

    // ===== 무기 박스 렌더링 =====
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

        bool isSelected = (i == selectedWeapon);
        bool isHovered  = (i == hoveredWeaponIndex);

        COLORREF bg, border;
        int borderWidth;
        if (isSelected)
        {
            bg = RGB(80, 90, 120);
            border = RGB(255, 220, 100);
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

        RENDER->SetText(18, RGB(20, 20, 20), TextAlign::Center);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(x + boxW * 0.5f, y + 10.f, weapons[i].name);

        if (weapons[i].image)
        {
            float imgSize = 50.f;
            float imgX = x + (boxW - imgSize) * 0.5f;
            float imgY = y + 40.f;
            RENDER->TransparentImage(
                weapons[i].image,
                imgX, imgY,
                imgX + imgSize, imgY + imgSize,
                RGB(255, 0, 255)
            );
        }
    }

    // ===== 하단 설명란 =====
    RenderCharacterDescription();
    RenderWeaponDescription();

    // 업그레이드 UI 렌더
    RenderUpgradeUI();

    // 시작 안내
    RENDER->SetText(20, RGB(30, 30, 30), TextAlign::Center);
    RENDER->Text(cx, CGame::WINSIZE.y - 30.f, TEXT("스페이스 또는 엔터로 시작"));
}

void CSceneTitle::RenderCharacterDescription()
{
    const float cx = CGame::WINSIZE.x * 0.5f;
    const float descY = CGame::WINSIZE.y - 220.f;
    const float descW = 300.f;
    const float descH = 80.f;
    const float descX = cx - descW - 20.f;  // 왼쪽에 배치

    RENDER->SetPen(PenType::Solid, RGB(120, 100, 140), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(45, 40, 60));
    RENDER->Rect(descX, descY, descX + descW, descY + descH);

    struct CharacterStats {
        wstring name;
        float hp;
        float attack;
        float speed;
        wstring special;
    };

    CharacterStats stats[2] = {
        { TEXT("Shana"),   6.f, 20.f, 200.f, TEXT("균형 잡힌 기본 캐릭터") },
        { TEXT("Diamond"), 5.f, 20.f, 150.f, TEXT("느린 속도와 낮은 체력, 쉬프트로 대시 가능") }
    };

    const CharacterStats& cs = stats[selectedCharacter];

    float textY = descY + 10.f;
    const float lineHeight = 18.f;

    RENDER->SetText(16, RGB(30, 30, 30), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(descX + descW * 0.5f, textY, cs.name);
    textY += lineHeight + 2.f;

    RENDER->SetText(12, RGB(40, 40, 40), TextAlign::Left);
    float col1X = descX + 15.f;
    float col2X = descX + descW * 0.5f + 10.f;

    RENDER->Text(col1X, textY, L"체력: " + to_wstring((int)cs.hp));
    RENDER->Text(col2X, textY, L"공격력: " + to_wstring((int)cs.attack));
    textY += lineHeight;

    RENDER->Text(col1X, textY, L"이동속도: " + to_wstring((int)cs.speed));
    textY += lineHeight;

    RENDER->SetText(11, RGB(40, 40, 40), TextAlign::Center);
    RENDER->Text(descX + descW * 0.5f, textY, cs.special);
}

void CSceneTitle::RenderWeaponDescription()
{
    const float cx = CGame::WINSIZE.x * 0.5f;
    const float descY = CGame::WINSIZE.y - 220.f;
    const float descW = 300.f;
    const float descH = 80.f;
    const float descX = cx + 20.f;  // 오른쪽에 배치

    RENDER->SetPen(PenType::Solid, RGB(100, 100, 130), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(40, 40, 55));
    RENDER->Rect(descX, descY, descX + descW, descY + descH);

    struct WeaponStats {
        wstring name;
        float damage;
        float rof;
        int count;
        int mag;
        wstring special;
    };

    WeaponStats stats[3] = {
        { TEXT("권총"),     15.f, 0.25f, 1, 6,  TEXT("균형 잡힌 기본 무기") },
        { TEXT("샷건"),     10.f, 0.25f, 4, 2,  TEXT("근거리 산탄") },
        { TEXT("기관단총"), 8.f,  0.125f, 1, 20, TEXT("빠른 연사") }
    };

    const WeaponStats& ws = stats[selectedWeapon];

    float textY = descY + 10.f;
    const float lineHeight = 18.f;

    RENDER->SetText(16, RGB(30, 30, 30), TextAlign::Center);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(descX + descW * 0.5f, textY, ws.name);
    textY += lineHeight + 2.f;

    RENDER->SetText(12, RGB(40, 40, 40), TextAlign::Left);
    float col1X = descX + 15.f;
    float col2X = descX + descW * 0.5f + 10.f;

    RENDER->Text(col1X, textY, L"피해량: " + to_wstring((int)ws.damage));
    float attackSpeed = (ws.rof > 0.001f) ? (1.0f / ws.rof) : 0.f;
    wchar_t speedBuf[32];
    swprintf_s(speedBuf, L"연사: %.1f/s", attackSpeed);
    RENDER->Text(col2X, textY, speedBuf);
    textY += lineHeight;

    RENDER->Text(col1X, textY, L"투사체: " + to_wstring(ws.count));
    RENDER->Text(col2X, textY, L"탄창: " + to_wstring(ws.mag));
    textY += lineHeight;

    RENDER->SetText(11, RGB(40, 40, 40), TextAlign::Center);
    RENDER->Text(descX + descW * 0.5f, textY, ws.special);
}

void CSceneTitle::RenderUpgradeUI()
{
    // Dummy upgrade (무한, 비용 500G)
    RENDER->SetPen(PenType::Solid, RGB(80, 80, 80), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(40, 40, 50));
    RENDER->Rect(upgradeDummyBox.left, upgradeDummyBox.top, upgradeDummyBox.right, upgradeDummyBox.bottom);

    RENDER->SetText(14, RGB(30, 30, 30), TextAlign::Left);
    RENDER->SetTextBackMode(TextBackMode::Null);
    const float pad = 12.f;
    RENDER->Text(upgradeDummyBox.left + pad, upgradeDummyBox.top + 12.f, L"재화 소비용 업그레이드");
    RENDER->SetText(12, RGB(60, 60, 60), TextAlign::Left);
    RENDER->Text(upgradeDummyBox.left + pad, upgradeDummyBox.top + 36.f, (L"레벨: " + to_wstring(dummyUpgradeCount)).c_str());

    // 우측 텍스트 대신 고정 비용 표시
    RENDER->SetText(12, RGB(180, 180, 40), TextAlign::Right);
    RENDER->Text(upgradeDummyBox.right - pad, upgradeDummyBox.top + 12.f, (L"Cost: " + to_wstring((int)kDummyCost) + L"G").c_str());

    // 홀드 진행 표시
    Vec2 mouse = INPUT->MouseScreenPos();
    bool hoveringDummy = (mouse.x >= upgradeDummyBox.left && mouse.x <= upgradeDummyBox.right &&
                          mouse.y >= upgradeDummyBox.top && mouse.y <= upgradeDummyBox.bottom);

    if (isHolding && holdingIndex == 0)
    {
        float prog = min(1.f, holdTimer / 1.0f);
        float barW = (upgradeDummyBox.right - upgradeDummyBox.left) - pad * 2;
        float barX = upgradeDummyBox.left + pad;
        float barY = upgradeDummyBox.bottom - 18.f;
        RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
        RENDER->SetBrush(BrushType::Null);
        RENDER->Rect(barX, barY, barX + barW, barY + 12.f);
        RENDER->SetBrush(BrushType::Solid, RGB(120, 200, 120));
        RENDER->Rect(barX + 1.f, barY + 1.f, barX + 1.f + (barW - 2.f) * prog, barY + 11.f);
    }
    else if (hoveringDummy)
    {
        RENDER->SetText(12, RGB(70, 70, 70), TextAlign::Center);
        RENDER->Text((upgradeDummyBox.left + upgradeDummyBox.right) * 0.5f, upgradeDummyBox.bottom - 18.f, L"꾹 누르면 구매 (1초)");
    }

    // Speed upgrade (최대 3회: +10 이동속도 each)
    RENDER->SetPen(PenType::Solid, RGB(80, 80, 80), 2);
    RENDER->SetBrush(BrushType::Solid, RGB(40, 40, 50));
    RENDER->Rect(upgradeSpeedBox.left, upgradeSpeedBox.top, upgradeSpeedBox.right, upgradeSpeedBox.bottom);

    RENDER->SetText(14, RGB(30, 30, 30), TextAlign::Left);
    RENDER->SetTextBackMode(TextBackMode::Null);
    RENDER->Text(upgradeSpeedBox.left + pad, upgradeSpeedBox.top + 12.f, L"이동속도 업그레이드");
    RENDER->SetText(12, RGB(60, 60, 60), TextAlign::Left);
    RENDER->Text(upgradeSpeedBox.left + pad, upgradeSpeedBox.top + 36.f, (L"레벨: " + to_wstring(speedUpgradeLevel) + L" / 3").c_str());

    // 비용 텍스트
    RENDER->SetText(12, RGB(180, 180, 40), TextAlign::Right);
    int nextCost = (speedUpgradeLevel < 3) ? speedCosts[speedUpgradeLevel] : 0;
    if (speedUpgradeLevel < 3)
        RENDER->Text(upgradeSpeedBox.right - pad, upgradeSpeedBox.top + 12.f, (L"Cost: " + to_wstring(nextCost) + L"G").c_str());
    else
        RENDER->Text(upgradeSpeedBox.right - pad, upgradeSpeedBox.top + 12.f, L"최대 레벨");

    bool hoveringSpeed = (mouse.x >= upgradeSpeedBox.left && mouse.x <= upgradeSpeedBox.right &&
                          mouse.y >= upgradeSpeedBox.top && mouse.y <= upgradeSpeedBox.bottom);

    if (isHolding && holdingIndex == 1)
    {
        float prog = min(1.f, holdTimer / 1.0f);
        float barW = (upgradeSpeedBox.right - upgradeSpeedBox.left) - pad * 2;
        float barX = upgradeSpeedBox.left + pad;
        float barY = upgradeSpeedBox.bottom - 18.f;
        RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
        RENDER->SetBrush(BrushType::Null);
        RENDER->Rect(barX, barY, barX + barW, barY + 12.f);
        RENDER->SetBrush(BrushType::Solid, RGB(120, 180, 220));
        RENDER->Rect(barX + 1.f, barY + 1.f, barX + 1.f + (barW - 2.f) * prog, barY + 11.f);
    }
    else if (hoveringSpeed)
    {
        RENDER->SetText(12, RGB(70, 70, 70), TextAlign::Center);
        RENDER->Text((upgradeSpeedBox.left + upgradeSpeedBox.right) * 0.5f, upgradeSpeedBox.bottom - 18.f, L"꾹 누르면 구매 (1초)");
    }

    // 재화 표시: 무기 박스 하단과 업그레이드 박스 상단 사이의 중앙에 배치 (가운데 정렬)
    wchar_t buf[64];
    swprintf_s(buf, L"Gold: %d G", CURRENCY->GetCurrency());
    float weaponsBottom = weaponBoxes[0].bottom;
    float descTop = CGame::WINSIZE.y - 220.f;
    float centerBetween = weaponsBottom + (descTop - weaponsBottom) * 0.5f;
    float uiTop = centerBetween - 80.f * 0.5f; // uiH == 80
    float currencyY = weaponsBottom + (uiTop - weaponsBottom) * 0.5f;
    RENDER->SetText(16, RGB(240, 220, 80), TextAlign::Center);
    RENDER->Text(CGame::WINSIZE.x * 0.5f, currencyY, buf);
}

void CSceneTitle::StartGame()
{
    // 무기 선택 적용
    if (selectedWeapon == 0)
        CSceneStage01::SetChosenWeapon(WeaponChoice::Pistol);
    else if (selectedWeapon == 1)
        CSceneStage01::SetChosenWeapon(WeaponChoice::Shotgun);
    else if (selectedWeapon == 2)
        CSceneStage01::SetChosenWeapon(WeaponChoice::SMG);

    // 캐릭터 선택 적용
    if (selectedCharacter == 0)
        CSceneStage01::SetChosenCharacter(CharacterChoice::Shana);
    else if (selectedCharacter == 1)
        CSceneStage01::SetChosenCharacter(CharacterChoice::Diamond);

    CAMERA->FadeOut(0.5f);
    EVENT->ChangeScene(SceneType::Stage01, 1.0f);
}

void CSceneTitle::Exit()
{
    SOUND->Stop(TEXT("Title_BGM"));
    // 씬 종료 시 저장
    SaveTitleUpgrades();
}

void CSceneTitle::Release()
{
    // 저장
    SaveTitleUpgrades();
}

// --- 업그레이드 저장/로드 ---
// 파일: PATH\title_upgrades.dat
void CSceneTitle::SaveTitleUpgrades()
{
    std::wstring path = PATH + std::wstring(L"\\title_upgrades.dat");
    std::ofstream ofs(path, std::ios::binary);
    if (ofs.is_open())
    {
        ofs.write(reinterpret_cast<const char*>(&dummyUpgradeCount), sizeof(dummyUpgradeCount));
        ofs.write(reinterpret_cast<const char*>(&speedUpgradeLevel), sizeof(speedUpgradeLevel));
        ofs.close();
        Logger::Debug(L"[CSceneTitle] Saved upgrades: dummy=" + to_wstring(dummyUpgradeCount) + L", speed=" + to_wstring(speedUpgradeLevel));
    }
    else
    {
        Logger::Debug(L"[CSceneTitle] Failed to save upgrades");
    }
}

void CSceneTitle::LoadTitleUpgrades()
{
    std::wstring path = PATH + std::wstring(L"\\title_upgrades.dat");
    std::ifstream ifs(path, std::ios::binary);
    if (ifs.is_open())
    {
        ifs.read(reinterpret_cast<char*>(&dummyUpgradeCount), sizeof(dummyUpgradeCount));
        ifs.read(reinterpret_cast<char*>(&speedUpgradeLevel), sizeof(speedUpgradeLevel));
        ifs.close();
        // 안전 범위 검사
        if (speedUpgradeLevel < 0) speedUpgradeLevel = 0;
        if (speedUpgradeLevel > 3) speedUpgradeLevel = 3;
        if (dummyUpgradeCount < 0) dummyUpgradeCount = 0;
        Logger::Debug(L"[CSceneTitle] Loaded upgrades: dummy=" + to_wstring(dummyUpgradeCount) + L", speed=" + to_wstring(speedUpgradeLevel));
    }
    else
    {
        // 파일 없음: 기본값 사용
        dummyUpgradeCount = 0;
        speedUpgradeLevel = 0;
        Logger::Debug(L"[CSceneTitle] No upgrade save found, defaults applied");
    }
}
