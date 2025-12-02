#include "pch.h"
#include "CTiledBackground.h"
#include "CGame.h"

CTiledBackground::CTiledBackground()
{
    name = TEXT("TiledBackground");
}

void CTiledBackground::Render()
{
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(39, 32, 48));
    RENDER->Rect(0, 0, CGame::WINSIZE.x, CGame::WINSIZE.y);

}