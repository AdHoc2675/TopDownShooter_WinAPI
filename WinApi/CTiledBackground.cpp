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

    for (int cy = cellStartY; cy < cellEndY; ++cy)
    {
        for (int cx = cellStartX; cx < cellEndX; ++cx)
        {
            float r = Hash01(cx, cy);
            if (r > decorDensity) // 배치 확률
                continue;

            int idx = PickDecorIndex(cx, cy);
            if (idx < 0) continue;

            // 셀 중앙 기준 배치 (약간 랜덤 오프셋)
            float jitterX = (Hash01(cx*7, cy*11) - 0.5f) * (decorCellW * 0.3f);
            float jitterY = (Hash01(cx*13, cy*5) - 0.5f) * (decorCellH * 0.3f);

            Vec2 worldPos(
                cx * (float)decorCellW + decorCellW * 0.5f + jitterX,
                cy * (float)decorCellH + decorCellH * 0.5f + jitterY
            );

            Vec2 scr = CAMERA->WorldToScreenPoint(worldPos);

            float x0 = scr.x - tileW * 0.5f;
            float y0 = scr.y - tileH * 0.5f;
            float x1 = x0 + (float)tileW;
            float y1 = y0 + (float)tileH;

            RENDER->TransparentImage(
                tiles[idx],
                x0, y0,
                x1, y1,
                RGB(255, 0, 255));
        }
    }
}