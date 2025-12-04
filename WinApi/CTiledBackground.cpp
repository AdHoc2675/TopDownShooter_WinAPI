#include "pch.h"
#include "CTiledBackground.h"
#include "CGame.h"
#include "CPlayer.h"

CTiledBackground::CTiledBackground()
{
    name = TEXT("TiledBackground");
    SetZOrder(100000.f); // 매우 뒤에 렌더링
}

static inline COLORREF lerpColor(COLORREF a, COLORREF b, float t)
{
    BYTE ar = GetRValue(a), ag = GetGValue(a), ab = GetBValue(a);
    BYTE br = GetRValue(b), bg = GetGValue(b), bb = GetBValue(b);
    BYTE r = (BYTE)(ar + (br - ar) * t);
    BYTE g = (BYTE)(ag + (bg - ag) * t);
    BYTE bch = (BYTE)(ab + (bb - ab) * t);
    return RGB(r, g, bch);
}

void CTiledBackground::Init()
{
    // 배경 타일 이미지 로드 (옵션)
    tileImg = LOADIMAGE(TEXT("T_BackTile"), TEXT("Image\\map.bmp"));

    // 고정 시드로 절차적 요소 1회 생성 (월드 좌표 기준)
    srand(12345);
    elems.clear();
    elems.reserve(400);

    // 맵 영역: 화면의 6배 범위에 분포
    const Vec2 mapHalf = CGame::WINSIZE * 3.f;

    auto rndf = [](float minv, float maxv) -> float {
        return minv + (float)(rand()) / (float)RAND_MAX * (maxv - minv);
    };

    // 요소 생성 함수
    auto pushElem = [&](BgElem::Type t, const Vec2& wp, float size, COLORREF base, int variant = 0) {
        BgElem e;
        e.type = t;
        e.worldPos = wp;
        e.size = size;
        e.color = base;
        e.variant = variant;
        elems.push_back(e);
    };

    // 배경 요소 기본 분포
    for (int i = 0; i < 220; ++i)
    {
        Vec2 wp(
            (float)((rand() % (int)(mapHalf.x * 2)) - (int)mapHalf.x),
            (float)((rand() % (int)(mapHalf.y * 2)) - (int)mapHalf.y));

        int pick = rand() % 4;
        if (pick == 0) // Rock
        {
            float sz = rndf(18.f, 46.f);
            COLORREF base = RGB(70 + rand() % 10, 62 + rand() % 10, 52 + rand() % 10);
            pushElem(BgElem::Rock, wp, sz, base, rand() % 3);
        }
        else if (pick == 1) // Bush
        {
            float sz = rndf(14.f, 34.f);
            COLORREF base = RGB(48 + rand() % 16, 78 + rand() % 14, 54 + rand() % 16);
            pushElem(BgElem::Bush, wp, sz, base, rand() % 2);
        }
        else if (pick == 2)// Pebble
        {
            float sz = rndf(6.f, 14.f);
            COLORREF base = RGB(86 + rand() % 10, 80 + rand() % 10, 72 + rand() % 10);
            pushElem(BgElem::Pebble, wp, sz, base, rand() % 4);
        }
        else if (pick == 3) // 버섯
        {
            float sz = rndf(20.f, 40.f);
            COLORREF base = RGB(230 + rand() % 15, 180 + rand() % 20, 80 + rand() % 10);
            pushElem(BgElem::Mushroom, wp, sz, base, rand() % 4);
        }

    }

    // 풀 덩어리(GrassPatch): 질감용 작은 타일 느낌
    for (int i = 0; i < 100; ++i)
    {
        Vec2 wp(
            (float)((rand() % (int)(mapHalf.x * 2)) - (int)mapHalf.x),
            (float)((rand() % (int)(mapHalf.y * 2)) - (int)mapHalf.y));

        float sz = rndf(16.f, 36.f);
        COLORREF base = RGB(50 + rand() % 10, 90 + rand() % 20, 50 + rand() % 10);
        pushElem(BgElem::GrassPatch, wp, sz, base, rand() % 3);
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
    RENDER->SetBrush(BrushType::Solid, RGB(7, 4, 9));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

    // 고정 월드 요소 렌더 (뷰포트 컬링 + 카메라 변환)
    const Vec2 screenSize = CGame::WINSIZE;
    const float margin = 32.f;

    
    
    // 플레이어 중심 비네트: player가 설정되어 있으면 중심 밝게
    if (player)
    {
        Vec2 center = CAMERA->WorldToScreenPoint(player->GetWorldPos());
        const float screenW = screenSize.x * 0.5;
        const float screenH = screenSize.y * 0.5;
        const float maxRadius = sqrtf(screenW * screenW + screenH * screenH) * 0.65f;

        const COLORREF centerLight = RGB(45, 42, 52);  // 더 밝게
        const COLORREF edgeDark    = RGB(7, 4, 9);  // 더 어둡게 (가장자리)

        // 밴드 수에 따라 부드러운 정도 조절, 이징 적용으로 부드러운 전환
        const int bands = 35;

        auto smoothstep = [](float x) {
            // 3차 스무스스텝: 3x^2 - 2x^3
            return x * x * (3.f - 2.f * x);
        };

        // 약간의 반지름 지터로 밴딩 완화(시각적 잡음)
        unsigned seed = 98765;
        auto jitter = [&]() {
            seed = seed * 1664525u + 1013904223u;
            return ((seed >> 16) & 0xFFFF) / 65535.f; // 0..1
        };

        RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);

        for (int i = bands; i >= 1; --i)
        {
            float t = (float)i / (float)bands;     // 1..0 (중심→가장자리)
            float eased = smoothstep(t);           // 부드러운 곡선으로
            float r = maxRadius * eased;

            // 1~2% 반지름 지터
            float j = (jitter() - 0.5f) * 0.02f;   // -1%..+1%
            r *= (1.f + j);

            // 색상 보간: 중심 밝은색 → 가장자리 어두운색 (보간 방향 반전)
            COLORREF col = lerpColor(centerLight, edgeDark, eased);

            RENDER->SetBrush(BrushType::Solid, col);
            
            // 화면 비율에 맞춰 약간 납작한 타원으로 깔기
            float rw = r;
            float rh = r;

            RENDER->Ellipse(center.x - rw, center.y - rh, center.x + rw, center.y + rh);
        }
    }
    
    for (const BgElem& e : elems)
    {
        Vec2 p = CAMERA->WorldToScreenPoint(e.worldPos);
        if (p.x < -margin || p.y < -margin || p.x > screenSize.x + margin || p.y > screenSize.y + margin)
            continue;

        switch (e.type)
        {
        case BgElem::Rock:
        {
            // 암석: 본체 + 하이라이트 + 그림자 타원
            float w = e.size * (0.6f + 0.05f * (e.variant));
            float h = e.size * 0.5f;
            COLORREF shade = lerpColor(e.color, RGB(20, 20, 20), 0.25f);
            COLORREF highlight = lerpColor(e.color, RGB(220, 220, 220), 0.15f);

            // 그림자
            RENDER->SetBrush(BrushType::Solid, RGB(30, 26, 22));
            RENDER->Ellipse(p.x - w * 0.65f, p.y - h * 0.35f, p.x + w * 0.65f, p.y + h * 0.35f);

            // 본체
            RENDER->SetBrush(BrushType::Solid, e.color);
            RENDER->Ellipse(p.x - w, p.y - h, p.x + w, p.y + h);

            // 하이라이트 덩어리
            RENDER->SetBrush(BrushType::Solid, highlight);
            RENDER->Ellipse(p.x - w * 0.45f, p.y - h * 0.6f, p.x - w * 0.05f, p.y - h * 0.2f);

            // 표면 요철 느낌의 작은 원 2개
            RENDER->SetBrush(BrushType::Solid, shade);
            RENDER->Circle(p.x + w * 0.2f, p.y + h * 0.1f, e.size * 0.12f);
            RENDER->Circle(p.x - w * 0.1f, p.y + h * 0.2f, e.size * 0.09f);
            break;
        }
        case BgElem::Bush:
        {
            // 덤불: 여러 겹 원 + 색조 변주
            COLORREF dark = lerpColor(e.color, RGB(20, 40, 20), 0.3f);
            COLORREF light = lerpColor(e.color, RGB(160, 200, 160), 0.2f);

            float r = e.size * 0.6f;
            RENDER->SetBrush(BrushType::Solid, e.color);
            RENDER->Circle(p.x, p.y, r);

            RENDER->SetBrush(BrushType::Solid, dark);
            RENDER->Circle(p.x - r * 0.6f, p.y + r * 0.25f, r * 0.75f);
            RENDER->Circle(p.x + r * 0.5f, p.y + r * 0.2f, r * 0.7f);

            // 상단 밝은 잎 덩어리
            RENDER->SetBrush(BrushType::Solid, light);
            RENDER->Circle(p.x - r * 0.2f, p.y - r * 0.4f, r * 0.4f);
            break;
        }
        case BgElem::Pebble:
        {
            // 자갈: 작은 원 + 반대편 작은 그림자 점
            float r = e.size * 0.5f;
            COLORREF shade = lerpColor(e.color, RGB(30, 30, 30), 0.25f);

            RENDER->SetBrush(BrushType::Solid, e.color);
            RENDER->Circle(p.x, p.y, r);

            RENDER->SetBrush(BrushType::Solid, shade);
            RENDER->Circle(p.x + r * 0.4f, p.y + r * 0.2f, r * 0.25f);
            break;
        }
        case BgElem::GrassPatch:
        {
            // 풀 패치: 작은 원 여러 개로 질감
            float s = e.size;
            COLORREF c1 = e.color;
            COLORREF c2 = lerpColor(e.color, RGB(30, 80, 30), 0.3f);
            COLORREF c3 = lerpColor(e.color, RGB(170, 220, 170), 0.15f);

            RENDER->SetBrush(BrushType::Solid, c1);
            RENDER->Circle(p.x, p.y, s * 0.40f);
            RENDER->SetBrush(BrushType::Solid, c2);
            RENDER->Circle(p.x - s * 0.25f, p.y + s * 0.10f, s * 0.30f);
            RENDER->Circle(p.x + s * 0.20f, p.y - s * 0.05f, s * 0.28f);
            RENDER->SetBrush(BrushType::Solid, c3);
            RENDER->Circle(p.x - s * 0.10f, p.y - s * 0.22f, s * 0.22f);
            break;
        }
        case BgElem::Mushroom:
        {
            // 버섯 크기
            float r = e.size * 0.4f;             // 갓의 반지름
            float stemW = r * 0.45f;             // 기둥 너비
            float stemH = r * 0.60f;             // 기둥 높이

            // 버섯 색상 (갓)
            COLORREF capBase = e.color;
            COLORREF capLight = lerpColor(capBase, RGB(255, 255, 220), 0.25f);
            COLORREF capDark = lerpColor(capBase, RGB(160, 120, 55), 0.35f);

            // 기둥 색상
            COLORREF stemCol = RGB(230, 220, 180);
            COLORREF stemShade = RGB(190, 180, 140);

            // 1) 버섯 그림자
            float shW = r * 1.2f;
            float shH = r * 0.45f;
            RENDER->SetBrush(BrushType::Solid, RGB(60, 50, 45));
            RENDER->Ellipse(p.x - shW, p.y + stemH * 0.4f, p.x + shW, p.y + shH + stemH * 0.4f);

            // 2) 기둥
            RENDER->SetBrush(BrushType::Solid, stemCol);
            RENDER->Rect(
                p.x - stemW * 0.5f,
                p.y,
                p.x + stemW * 0.5f,
                p.y + stemH
            );

            // 3) 기둥 그림자
            RENDER->SetBrush(BrushType::Solid, stemShade);
            RENDER->Rect(
                p.x,
                p.y,
                p.x + stemW * 0.5f,
                p.y + stemH
            );

            // 4) 버섯 갓 (큰 타원)
            RENDER->SetBrush(BrushType::Solid, capBase);
            RENDER->Ellipse(
                p.x - r,
                p.y - r * 0.8f,
                p.x + r,
                p.y + r * 0.4f
            );

            // 5) 갓의 밝은 부분 (하이라이트)
            RENDER->SetBrush(BrushType::Solid, capLight);
            RENDER->Ellipse(
                p.x - r * 0.6f,
                p.y - r * 0.7f,
                p.x - r * 0.1f,
                p.y - r * 0.2f
            );

            // 6) 갓의 점 (variant로 변주)
            int dotCount = 3 + (e.variant % 3);
            for (int i = 0; i < dotCount; ++i)
            {
                float dx = ((rand() % 100) / 100.f - 0.5f) * r;
                float dy = ((rand() % 100) / 100.f - 0.4f) * r * 0.6f;

                RENDER->SetBrush(BrushType::Solid, capDark);
                RENDER->Circle(p.x + dx, p.y + dy, r * 0.15f);
            }

            break;
        }


        default:
            break;
        }
    }
}