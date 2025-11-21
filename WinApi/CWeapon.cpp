#include "pch.h"
#include "CWeapon.h"
#include "CMissile.h"

CWeapon::CWeapon()
{
	name  = TEXT("무기");
	scale = Vec2(30, 100);
}

CWeapon::~CWeapon()
{
}

void CWeapon::Init()
{
}

void CWeapon::OnEnable()
{
}

void CWeapon::Update()
{
	// 쿨다운 감소
	if (curCooldown > 0.f)
		curCooldown -= DT;

	// 왼쪽 마우스 버튼 클릭 시 단발
	if (curCooldown <= 0.f && INPUT->ButtonDown(VK_LBUTTON))
	{
		FireToCursor();
		curCooldown = fireCooldown;
	}

	// 스페이스로 5발 스프레드
	if (curCooldown <= 0.f && INPUT->ButtonDown(VK_SPACE))
	{
		FireSpreadToCursor(5, 30.f); // 30도 범위로 5발
		curCooldown = fireCooldown;
	}
}

void CWeapon::Render()
{
	RENDER->Rect(
		renderPos.x - scale.x * 0.5f,
		renderPos.y - scale.y * 0.5f,
		renderPos.x + scale.x * 0.5f,
		renderPos.y + scale.y * 0.5f);
}

void CWeapon::OnDisable()
{
}

void CWeapon::Release()
{
}

void CWeapon::FireToCursor()
{
	Vec2 weaponWorld = worldPos;
	Vec2 targetWorld = INPUT->MouseWorldPos();

	Vec2 dir = targetWorld - weaponWorld;
	if (dir.Length() <= 0.0001f)
		dir = Vec2(0, -1); // 동일 위치 클릭 시 기본 방향

	dir = dir.Normalized();

	// 총구 앞 (무기 길이의 절반 + 약간)으로 스폰 오프셋
	float spawnDistance = scale.y * 0.5f + 10.f;
	Vec2 spawnPos = weaponWorld + dir * spawnDistance;

	CreateMissile(spawnPos, dir);
}

void CWeapon::FireSpreadToCursor(int count, float spreadAngleDeg)
{
	if (count <= 1)
	{
		FireToCursor();
		return;
	}

	Vec2 weaponWorld = worldPos;
	Vec2 targetWorld = INPUT->MouseWorldPos();
	Vec2 baseDir = targetWorld - weaponWorld;
	if (baseDir.Length() <= 0.0001f)
		baseDir = Vec2(0, -1);
	baseDir = baseDir.Normalized();

	// 중앙 방향을 기준으로 좌우로 각도 분배
	float half = (count - 1) * 0.5f;
	float step = (count > 1) ? spreadAngleDeg / (count - 1) : 0.f;

	float spawnDistance = scale.y * 0.5f + 10.f;

	for (int i = 0; i < count; ++i)
	{
		float offsetIndex = i - half; // -half ... +half
		float angleDeg = offsetIndex * step;
		float angleRad = angleDeg * 3.141592f / 180.f;

		// 회전 (x,y) -> (x*cos - y*sin, x*sin + y*cos)
		Vec2 dir;
		dir.x = baseDir.x * cosf(angleRad) - baseDir.y * sinf(angleRad);
		dir.y = baseDir.x * sinf(angleRad) + baseDir.y * cosf(angleRad);
		dir = dir.Normalized();

		Vec2 spawnPos = weaponWorld + dir * spawnDistance;
		CreateMissile(spawnPos, dir);
	}
}

void CWeapon::CreateMissile(const Vec2& spawnPos, const Vec2& dir)
{
	CMissile* missile = new CMissile();
	missile->SetPos(spawnPos);
	missile->SetDir(dir);
	EVENT->AddGameObject(GetScene(), missile);
}
