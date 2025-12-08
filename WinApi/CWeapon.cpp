#include "pch.h"
#include "CWeapon.h"
#include "CMissile.h"
#include "CGame.h"
#include "CPlayer.h"

using namespace std;

CWeapon::CWeapon()
{
	name  = TEXT("무기");
	scale = Vec2(10, 25);
	fireCooldown = 0.25f;      // 발사 간 최소 간격(초)
	curCooldown = 0.f;
	maxChamberSize = 6.f;    // 탄창 크기
	curChamberSize = 6.f; // 현재 남은 탄창 크기
	reloadTime = 1.f;          // 재장전 시간(초)
	curReloadTime = 0.f;
	projectileCount = 1;     // 투사체 수
	spreadAngleDeg = 10.f;  // 산탄 각도
	damage = 15.f;  // 무기 대미지 기본값
	pierceCount = 0; 		 // 미사일 관통 횟수
	missileSpeedMultiplier = 1.f; // 미사일 이동속도 배율
}

CWeapon::~CWeapon()
{
}

void CWeapon::Init()
{
	fireSound = LOADSOUND(TEXT("GunFire"), TEXT("Sound\\gun_fire.wav"));
	reloadSound = LOADSOUND(TEXT("GunReload"), TEXT("Sound\\gun_reload.wav"));
}

void CWeapon::OnEnable()
{
	// 플레이어가 연결되어 있다면 장착 무기로 설정하여 UI에 반영되도록 함
	if (player) {
		player->SetWeapon(this);
	}
}

void CWeapon::Update()
{
	// 쿨다운 감소
	if (curCooldown > 0.f)
		curCooldown = curCooldown - DT;

	// 재장전 진행 중이면 타이머 갱신
	if (curReloadTime > 0.f)
	{
		curReloadTime = curReloadTime - DT;
		if (curReloadTime <= 0.f)
		{
			curReloadTime = 0.f;
			curChamberSize = maxChamberSize; // 재장전 완료
		}
		return; // 재장전 중에는 발사 불가
	}

	// 수동 재장전 (R) - 탄이 가득 차 있지 않을 때만
	if (INPUT->ButtonDown('R') && curChamberSize < maxChamberSize)
	{
		if (reloadSound) {
			SOUND->PlayOnce(reloadSound);
		}

		curReloadTime = reloadTime;
		return;
	}

	// 좌클릭으로 사격
	if (curCooldown <= 0.f && INPUT->ButtonStay(VK_LBUTTON))
	{
		if (curChamberSize >= 1.f)
		{
			// 탄 소모는 발사체 수와 무관
			curChamberSize = curChamberSize - 1.f;

			// 스프레드 발사
			int count = (projectileCount <= 0) ? 1 : projectileCount;
			float angle = (spreadAngleDeg < 0.f) ? 0.f : spreadAngleDeg;
			FireSpreadToCursor(count, angle);

			if (fireSound)
				SOUND->PlayOnce(fireSound);

			curCooldown = fireCooldown;

			// 탄 다 쓰면 자동 재장전
			if (curChamberSize <= 0.f) {
				if (reloadSound) {
					SOUND->PlayOnce(reloadSound);
				}
				curReloadTime = reloadTime;
			}
		}
		else
		{
			// 빈 탄창이면 재장전 시작
			if (reloadSound) {
				SOUND->PlayOnce(reloadSound);
			}
			curReloadTime = reloadTime;
		}
	}

	// 스페이스: 스프레드 사격 (남은 탄 수만큼만 발사)
	if (curCooldown <= 0.f && INPUT->ButtonStay(VK_SPACE))
	{
		const int desired = 1;
		int bullets = (int)curChamberSize;
		if (bullets > desired) bullets = desired;

		if (bullets > 0)
		{
			curChamberSize -= (float)bullets;

			FireSpreadToCursor(bullets, 30.f); // 30도 범위로 발사

			if (fireSound)
				SOUND->PlayOnce(fireSound);

			curCooldown = fireCooldown;

			if (curChamberSize <= 0.f) {
				if (reloadSound) {
					SOUND->PlayOnce(reloadSound);
				}
				curReloadTime = reloadTime;
			}
		}
		else
		{
			if (reloadSound) {
				SOUND->PlayOnce(reloadSound);
			}

			curReloadTime = reloadTime;
		}
	}
}

void CWeapon::Render()
{

	// 무기 본체 렌더링 (흰색 사각형)
	RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
	RENDER->SetBrush(BrushType::Solid, RGB(255, 255, 255));

	RENDER->Rect(
		renderPos.x - scale.x * 0.5f,
		renderPos.y - scale.y * 0.5f,
		renderPos.x + scale.x * 0.5f,
		renderPos.y + scale.y * 0.5f);
	
	// 남은 탄 수 표시
	int textSize = 24;
	RENDER->SetText(textSize, RGB(20, 20, 20), TextAlign::Left);
	RENDER->SetTextBackMode(TextBackMode::Null);

	// 현재 탄/최대 탄
	wstring ammoText = L"AMMO: " + to_wstring((int)curChamberSize) +
		L" / " + to_wstring((int)maxChamberSize);

	const float hudX = 10.f;
	const float hudY = CGame::WINSIZE.y * 0.9f - (float)textSize;
	RENDER->Text(hudX, hudY, ammoText);

	// 재장전 중이면 막대 진행바의 형태로 남은 시간 표시
	if (curReloadTime > 0.f)
	{
		float progress = 1.f - (curReloadTime / reloadTime); // 0→1

		// 무기 위쪽 위치
		float barWidth = 100.f;
		float barHeight = 10.f;
		float offsetY = scale.y * 0.5f + 25.f; // 머리 위 여백 간격
		float barX = renderPos.x - barWidth * 0.5f;
		float barY = renderPos.y - offsetY - barHeight;

		// 테두리바
		RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
		RENDER->SetBrush(BrushType::Null);
		RENDER->Rect(barX, barY, barX + barWidth, barY + barHeight);

		// 최소 폭 보호
		float fillW = barWidth * progress;
		if (fillW < 2.f && progress > 0.f) fillW = 2.f;

		// 진행 색상 일단은 검은색
		COLORREF fillColor = RGB(0, 0, 0);

		RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
		RENDER->SetBrush(BrushType::Solid, fillColor);
		RENDER->Rect(barX + 1.f, barY + 1.f, barX + fillW - 1.f, barY + barHeight - 1.f);
	}
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
	if (count <= 0)
		return;

	// 1발이면 중앙으로 발사
	if (count == 1)
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

	// 간격 모드: spreadAngleDeg를 '인접 투사체 간 각도'로 해석
	float step = spreadAngleDeg; // 인접 간격(도)
	float half = (count - 1) * 0.5f;
	float spawnDistance = scale.y * 0.5f + 10.f;

	for (int i = 0; i < count; i++)
	{
		float offsetIndex = i - half;             // -half ... +half
		float angleDeg = offsetIndex * step;      // 고정 간격으로 분산
		float angleRad = angleDeg * 3.141592f / 180.f;

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

	if (player)
	{
		missile->InheritCombat(player->GetCombatStats());
		CombatStats& mstats = missile->GetCombatStats();
		mstats.attack = damage;
	}

	missile->SetPierceCount(pierceCount);
	missile->SetMoveSpeed(500.f * missileSpeedMultiplier);

	// 발화 설정 전달 (확률 포함)
	if (appliesBurn)
	{
		missile->SetAppliesBurn(true, burnStacks, burnDuration, burnChance);
	}

	EVENT->AddGameObject(GetScene(), missile);
}
