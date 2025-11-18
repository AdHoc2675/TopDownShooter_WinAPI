#include "pch.h"
#include "CCameraController.h"

#include "CPlayer.h"

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
}

void CCameraController::OnEnable()
{
}

void CCameraController::Update()
{
	//if (INPUT->ButtonDown(VK_LBUTTON))
	//{
	//	CAMERA->SetTargetPos(INPUT->MouseWorldPos(), 1);
	//}

	//Vec2 dir;
	//if (INPUT->ButtonStay('A'))
	//	dir.x = -1;
	//else if (INPUT->ButtonStay('D'))
	//	dir.x = 1;
	//else
	//	dir.x = 0;

	//if (INPUT->ButtonStay('W'))
	//	dir.y = -1;
	//else if (INPUT->ButtonStay('S'))
	//	dir.y = 1;
	//else
	//	dir.y = 0;

	//CAMERA->Scroll(dir, 200);

	if (m_player == nullptr)
		return;

	// Player world position
	Vec2 playerPos = m_player->GetWorldPos();

	Vec2 mousePos = INPUT->MouseWorldPos(); // mouse world position

	// the point between playerPos and mousePos
	Vec2 midPos = (playerPos + mousePos) * 0.5f;

	// set camera target position to midPos
	// timeToTarget smaller -> camera follows more smoothly
	float timeToTarget = 0.15f;  
	CAMERA->SetTargetPos(midPos, timeToTarget);

}

void CCameraController::Render()
{
}

void CCameraController::OnDisable()
{
}

void CCameraController::Release()
{
}
