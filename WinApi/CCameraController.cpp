#include "pch.h"
#include "CCameraController.h"

#include "CPlayer.h"
#include "CImage.h"

CCameraController::CCameraController()
	: m_player(nullptr)
{
}

CCameraController::~CCameraController()
{
}

void CCameraController::Init()
{
	CAMERA->SetTargetObj(nullptr);
	m_cursorImage = LOADIMAGE(TEXT("Cursor"), TEXT("Image\\CurserImage.bmp"));
}

void CCameraController::OnEnable()
{
}

void CCameraController::Update()
{
	if (m_player == nullptr)
		return;

	Vec2 playerPos = m_player->GetWorldPos();
	Vec2 mousePos = INPUT->MouseWorldPos();

	// Point closer to player (1/3 point) by dividing player position and mouse position into three equal parts
	Vec2 focusPos = playerPos + (mousePos - playerPos) * (1.0f / 3.0f);

	float timeToTarget = 0.2f;
	CAMERA->SetTargetPos(focusPos, timeToTarget);

}

void CCameraController::Render()
{
	if (m_cursorImage) {
        Vec2 mouseWorld = INPUT->MouseWorldPos();
        Vec2 mouseScreen = CAMERA->WorldToScreenPoint(mouseWorld);

        float scale = 0.5f; // 50% 크기
        float w = (float)m_cursorImage->GetBmpWidth() * scale;
        float h = (float)m_cursorImage->GetBmpHeight() * scale;

        // 중심 정렬 (마우스 위치를 이미지 중앙에 맞춤)
        float startX = mouseScreen.x - w * 0.5f;
        float startY = mouseScreen.y - h * 0.5f;
        float endX = startX + w;
        float endY = startY + h;

        // 마젠타(RGB(255,0,255)) 투명 처리
        RENDER->TransparentImage(
            m_cursorImage,
            startX, startY,
            endX, endY,
            RGB(255, 0, 255)
        );
	}
}

void CCameraController::OnDisable()
{
}

void CCameraController::Release()
{
}
