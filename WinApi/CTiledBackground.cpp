#include "pch.h"
#include "CTiledBackground.h"
#include "CGame.h"
#include "CPlayer.h"

CTiledBackground::CTiledBackground()
{
    name = TEXT("TiledBackground");
	SetZOrder(100000.f); // 매우 뒤에 렌더링
}

void CTiledBackground::Init()
{
    // 배경 타일 이미지 로드 (프로젝트 리소스에 맞게 교체)
    // 이미지가 없으면 단색만 렌더됨
    tileImg = LOADIMAGE(TEXT("T_BackTile"), TEXT("Image\\map.bmp"));

    // 고정 시드로 절차적 요소 1회 생성 (월드 좌표 기준)
    srand(12345);
    elems.reserve(200);
    // 맵을 가정: 화면 크기의 몇 배 영역을 배경으로 배치
    const Vec2 mapHalf = CGame::WINSIZE * 3.f; // 화면의 6배 영역
    for (int i = 0; i < 200; ++i)
    {
        BgElem e;
        int t = rand() % 3;
        e.type = static_cast<BgElem::Type>(t);
        e.worldPos.x = (float)((rand() % (int)(mapHalf.x * 2)) - (int)mapHalf.x);
        e.worldPos.y = (float)((rand() % (int)(mapHalf.y * 2)) - (int)mapHalf.y);

        switch (e.type)
        {
        case BgElem::Rock:
            e.size = 18.f + (rand() % 22);  // 18~40
            e.color = RGB(70, 62, 52);
            break;
        case BgElem::Bush:
            e.size = 14.f + (rand() % 18);  // 14~32
            e.color = RGB(56, 84, 60);
            break;
        case BgElem::Pebble:
        default:
            e.size = 6.f + (rand() % 8);    // 6~14
            e.color = RGB(90, 82, 72);
            break;
        }
        elems.push_back(e);
    }

}

void CTiledBackground::Update()
{
    // 배경은 상태 변화가 없으므로 로직 없음
}

void CTiledBackground::Render()
{
    // 기본 단색 배경
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(39, 32, 48));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

    // 고정 월드 요소 렌더 (뷰포트 컬링 + 카메라 변환)
    const Vec2 screenSize = CGame::WINSIZE;
    for (const BgElem& e : elems)
    {
        Vec2 p = CAMERA->WorldToScreenPoint(e.worldPos);

        // 화면 밖이면 스킵 (약간의 마진으로 가장자리 보정)
        const float m = 24.f;
        if (p.x < -m || p.y < -m || p.x > screenSize.x + m || p.y > screenSize.y + m)
            continue;

        RENDER->SetBrush(BrushType::Solid, e.color);
        switch (e.type)
        {
        case BgElem::Rock:
            // 바위: 타원으로 변형
            RENDER->Ellipse(p.x - e.size * 0.6f, p.y - e.size * 0.5f,
                            p.x + e.size * 0.6f, p.y + e.size * 0.5f);
            break;
        case BgElem::Bush:
            // 덤불: 원 2~3개 겹치기
            RENDER->Circle(p.x, p.y, e.size * 0.6f);
            RENDER->Circle(p.x - e.size * 0.35f, p.y + e.size * 0.2f, e.size * 0.45f);
            RENDER->Circle(p.x + e.size * 0.35f, p.y + e.size * 0.1f, e.size * 0.5f);
            break;
        case BgElem::Pebble:
        default:
            // 자갈: 작은 원
            RENDER->Circle(p.x, p.y, e.size * 0.5f);
            break;
        }
    }
}