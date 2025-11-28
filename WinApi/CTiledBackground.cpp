#include "pch.h"
#include "CTiledBackground.h"
#include "CGame.h"

CTiledBackground::CTiledBackground()
{
    name = TEXT("TiledBackground");
}

float CTiledBackground::Hash01(int x, int y) const
{
    // 좌표 기반 결정적 난수 0~1
    uint32_t h = 2166136261u;
    h = (h ^ (uint32_t)x) * 16777619u;
    h = (h ^ (uint32_t)y) * 16777619u;
    h ^= (h >> 13);
    h *= 1274126177u;
    h ^= (h >> 16);
    return (h & 0xFFFFFF) / 16777215.f;
}

int CTiledBackground::PickDecorIndex(int cellX, int cellY) const
{
    if (tiles.empty()) return -1;
    // 단순 해시 기반 선택
    uint32_t h = 2166136261u;
    h = (h ^ (uint32_t)cellX) * 16777619u;
    h = (h ^ (uint32_t)cellY) * 16777619u;
    return (int)(h % tiles.size());
}

void CTiledBackground::Render()
{
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(39, 32, 48));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

    if (!sparseDecor || tiles.empty())
        return;

    // 화면 월드 영역 확장
    Vec2 worldTL = CAMERA->ScreenToWorldPoint(Vec2(0.f, 0.f));
    Vec2 worldBR = CAMERA->ScreenToWorldPoint(CGame::WINSIZE);
    worldTL.x -= margin; worldTL.y -= margin;
    worldBR.x += margin; worldBR.y += margin;

    // 데코 셀 범위
    int cellStartX = (int)floorf(worldTL.x / decorCellW);
    int cellStartY = (int)floorf(worldTL.y / decorCellH);
    int cellEndX   = (int)ceilf(worldBR.x / decorCellW);
    int cellEndY   = (int)ceilf(worldBR.y / decorCellH);

    
}