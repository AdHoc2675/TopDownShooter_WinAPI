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

    //RENDER->TransparentImage(
    //    tileImg,
    //    0, 0,    // 시작 좌표
    //    tileImg->GetBmpWidth(), tileImg->GetBmpHeight(),    // 끝 좌표 (width/height 아님)
    //    RGB(255, 0, 255)); // 마젠타 투명색
}