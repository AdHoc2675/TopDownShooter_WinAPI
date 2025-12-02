#include "pch.h"
#include "CTiledBackground.h"
#include "CGame.h"
#include "CPlayer.h"

CTiledBackground::CTiledBackground()
{
    name = TEXT("TiledBackground");
    tileImg = nullptr;
    tileWorldSize = 0.f;
    gridStep = 0.f;
    density = 0.f;
    colorKey = RGB(255, 0, 255);
    noiseSeed = 0x9E3779B9u; // 임의 기본 시드
}

void CTiledBackground::Init()
{
    // 배경 타일 이미지 로드 (프로젝트 리소스에 맞게 교체)
    // 이미지가 없으면 단색만 렌더됨
    tileImg = LOADIMAGE(TEXT("T_BackTile"), TEXT("Image\\T_ForestTile_3.bmp"));

    // 월드 그리드 파라미터
    tileWorldSize = 96.f;   // 타일 표시 크기(월드 픽셀 기준 한 변)
    gridStep      = 220.f;  // 그리드 칸 간격(타일 사이 빈 공간 포함)
    density       = 0.15f;  // 각 칸에 타일을 놓을 확률(낮게 유지해 드문드문)
    colorKey      = RGB(255, 0, 255);

    // 해시 시드 (장면마다 바꾸고 싶으면 다른 값 주입)
    noiseSeed = 0xA3B1C2D3u;
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

    // 월드 기준 타일 렌더링 (CPU/메모리 부담 최소화)
    RenderWorldTiles();
}

// 화면에 보이는 월드 영역에 대해 그리드 좌표를 순회하며 확률적으로 타일을 그립니다.
void CTiledBackground::RenderWorldTiles()
{
    if (!tileImg) return;

    // 카메라 오프셋(플레이어 기준) 이용
    CPlayer* p = GetPlayer();
    if (!p) return;

    // 화면 네 모서리의 월드 좌표를 추정
    Vec2 camShift = p->GetWorldPos() - p->GetRenderPos();
    Vec2 worldTopLeft  = camShift;
    Vec2 worldBottomRight = camShift + Vec2(CGame::WINSIZE.x, CGame::WINSIZE.y);

    // 그리드 인덱스 범위 계산
    const float step = gridStep;
    int gxStart = (int)floorf(worldTopLeft.x / step) - 1;
    int gyStart = (int)floorf(worldTopLeft.y / step) - 1;
    int gxEnd   = (int)ceilf(worldBottomRight.x / step) + 1;
    int gyEnd   = (int)ceilf(worldBottomRight.y / step) + 1;

    // 타일 크기와 그리기 좌표 계산용
    const float w = tileWorldSize;
    const float h = tileWorldSize;

    // 화면에 보이는 범위의 그리드 칸만 순회
    for (int gy = gyStart; gy <= gyEnd; ++gy)
    {
        for (int gx = gxStart; gx <= gxEnd; ++gx)
        {
            // 결정적 확률 테스트 (메모리 사용 없이 일관된 배치)
            if (Hash01(gx, gy) > density)
                continue;

            // 그리드 기준 월드 위치(중심 배치)
            Vec2 worldPosCenter = Vec2(gx * step, gy * step);

            // 월드 → 스크린 변환
            Vec2 screenPos = WorldToScreen(worldPosCenter);

            // 타일의 화면 사각형(중심 기준) 계산
            float dstStartX = screenPos.x - w * 0.5f;
            float dstStartY = screenPos.y - h * 0.5f;
            float dstEndX   = dstStartX + w;
            float dstEndY   = dstStartY + h;

            // 컬러키 투명 렌더
            RENDER->TransparentImage(tileImg, dstStartX, dstStartY, dstEndX, dstEndY, colorKey);
        }
    }
}

// 결정적 해시 기반 의사난수 (좌표와 시드로부터 0~1 반환)
float CTiledBackground::Hash01(int gx, int gy) const
{
    // 간단한 2D 해시 -> 32비트 정수 -> [0,1] float
    unsigned int x = (unsigned int)gx;
    unsigned int y = (unsigned int)gy;
    unsigned int h = noiseSeed;
    h ^= x * 0x9E3779B9u + 0x7F4A7C15u;
    h ^= (y ^ (y << 16)) * 0x85EBCA6Bu;
    h ^= h >> 13;
    h *= 0xC2B2AE35u;
    h ^= h >> 16;
    return (h & 0x00FFFFFFu) / 16777215.0f; // 0~1
}

// 플레이어 카메라 오프셋을 이용한 월드→스크린 변환
Vec2 CTiledBackground::WorldToScreen(const Vec2& w) const
{
    // 플레이어가 카메라의 기준이라고 가정
    CPlayer* p = GetPlayer();
    if (!p) return w;

    Vec2 camShift = p->GetWorldPos() - p->GetRenderPos();
    return w - camShift;
}