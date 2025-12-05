#include "pch.h"
#include "CSMGWeapon.h"

CSMGWeapon::CSMGWeapon()
{
    // 표시 이름
    name = TEXT("SMG");
    // 외형(사각형 렌더 기준) 기본 크기
    scale = Vec2(12.f, 28.f);

    // 기본 성능
    damage = 10.f;               // 피해
    fireCooldown = 0.125f;         // 연사 속도(초)
    projectileCount = 1;         // 발사체 수
    maxChamberSize = 20.f;        // 최대 탄창
    curChamberSize = maxChamberSize;
    reloadTime = 2.0f;           // 재장전 시간(초)
    pierceCount = 0;             // 기본 관통 1회
    spreadAngleDeg = 10.f;
    missileSpeedMultiplier = 1.f;

    // 내부 쿨다운/재장전 타이머 초기화
    curCooldown = 0.f;
    curReloadTime = 0.f;
}
