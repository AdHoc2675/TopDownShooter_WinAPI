#pragma once
#include "pch.h"
#include "CExplosionEffect.h"
#include "CMonster.h"
#include "CPlayer.h"
#include "CCombatSystem.h"
#include "CDamageText.h"

CExplosionEffect::CExplosionEffect()
{
    name = TEXT("폭발 이펙트");
    scale = Vec2(160, 160);
    
    // 기본 전투 스탯
    stats.attack = 20.f;
    stats.hp = 1.f;
    stats.maxHp = 1.f;
    stats.defense = 0.f;
    stats.critChance = 0.1f;
    stats.critMultiplier = 1.5f;
}

CExplosionEffect::~CExplosionEffect()
{
}

void CExplosionEffect::Configure(const Vec2& center, float radius, float damage, 
                                 bool isFriendly, float life)
{
    SetPos(center);
    explosionRadius = radius;
    lifetime = life;
    maxLifetime = life;
    friendly = isFriendly;
    hasDealtDamage = false;
    
    stats.attack = damage;
    
    // 스케일도 반경에 맞춰 조정
    scale = Vec2(radius * 2.f, radius * 2.f);
}

void CExplosionEffect::Init()
{
    // 폭발 범위만큼 콜라이더 생성
    collider = new CCollider();
    collider->SetScale(Vec2(explosionRadius * 2.f, explosionRadius * 2.f));
    
    // 레이어 설정: 아군/적군 구분
    if (friendly)
        collider->SetLayer(Layer::Missile); // 아군 폭발 -> 몬스터에 피해
    else
        collider->SetLayer(Layer::Monster); // 적군 폭발 -> 플레이어에 피해
    
    AddChild(collider);
}

void CExplosionEffect::OnEnable()
{
}

void CExplosionEffect::Update()
{
    lifetime -= DT;
    if (lifetime <= 0.f)
    {
        EVENT->Delete(GetScene(), this);
    }
    
    // 콜라이더 크기를 현재 scale에 맞춰 업데이트 (확장 애니메이션)
    if (collider)
    {
        collider->SetScale(scale);
    }
}

void CExplosionEffect::Render()
{
    // 시간에 따라 확장되는 원 효과
    float progress = 1.0f - (lifetime / maxLifetime); // 0→1
    float currentRadius = explosionRadius * (0.5f + 0.5f * progress); // 50%~100% 확장
    float alpha = 1.0f - progress; // 1→0 (페이드아웃)
    
    // 외곽 원 (주황/빨강)
    COLORREF outerColor = RGB(255, (int)(100 * alpha), 0);
    RENDER->SetPen(PenType::Solid, outerColor, (int)(4 * alpha + 1));
    RENDER->SetBrush(BrushType::Null);
    RENDER->Circle(renderPos.x, renderPos.y, currentRadius);
    
    // 내부 원 (노랑)
    if (progress < 0.5f)
    {
        float innerAlpha = alpha * (1.0f - progress * 2.0f);
        COLORREF innerColor = RGB(255, 255, (int)(200 * innerAlpha));
        RENDER->SetPen(PenType::Null, RGB(0,0,0), 0);
        RENDER->SetBrush(BrushType::Solid, innerColor);
        RENDER->Circle(renderPos.x, renderPos.y, currentRadius * 0.6f);
    }
}

void CExplosionEffect::OnDisable()
{
}

void CExplosionEffect::Release()
{
}

void CExplosionEffect::OnCollisionEnter(CCollider* other)
{
    if (!other)
        return;
    
    // 이미 피해를 줬다면 중복 방지 (한 프레임에 여러 충돌 발생 가능)
    // 실제로는 매 프레임 새로운 충돌 대상에 피해를 줄 수 있도록
    // 개별 대상 추적이 필요하지만, 짧은 수명(0.3초)이라 단순화
    
    CGameObject* target = other->GetOwner();
    if (!target)
        return;
    
    // 아군 폭발: 몬스터에 피해
    if (friendly && other->GetLayer() == Layer::Monster)
    {
        CMonster* monster = dynamic_cast<CMonster*>(target);
        if (!monster)
            return;
        
        CombatStats& victimStats = monster->GetCombatStats();
        if (!victimStats.alive())
            return;
        
        // 거리 기반 피해 감쇠
        Vec2 diff = monster->GetWorldPos() - worldPos;
        float distance = diff.Length();
        float damageMultiplier = 1.0f;
        
        if (distance > 0.f && explosionRadius > 0.f)
        {
            float distanceRatio = distance / explosionRadius;
            damageMultiplier = 1.0f - (distanceRatio * 0.5f);
            if (damageMultiplier < 0.5f) damageMultiplier = 0.5f;
        }
        
        CombatStats adjustedStats = stats;
        adjustedStats.attack *= damageMultiplier;
        
        float dealt = 0.f;
        bool crit = false;
        COMBAT->ApplyDamage(this, monster, adjustedStats, victimStats, &dealt, &crit);
        
        CDamageText* dt = new CDamageText();
        dt->Configure(monster->GetWorldPos(), (int)dealt, crit);
        EVENT->AddGameObject(GetScene(), dt);
    }
    // 적군 폭발: 플레이어에 피해
    else if (!friendly && other->GetLayer() == Layer::Player)
    {
        CPlayer* player = dynamic_cast<CPlayer*>(target);
        if (!player)
            return;
        
        // 플레이어 hitCooldown 체크 추가
        if (player->IsHitCooldown())
            return; // 쿨다운 중이면 피해 없음
        
        CombatStats& victimStats = player->GetCombatStats();
        if (!victimStats.alive())
            return;
        
        // 거리 기반 피해 감쇠
        Vec2 diff = player->GetWorldPos() - worldPos;
        float distance = diff.Length();
        float damageMultiplier = 1.0f;
        
        if (distance > 0.f && explosionRadius > 0.f)
        {
            float distanceRatio = distance / explosionRadius;
            damageMultiplier = 1.0f - (distanceRatio * 0.5f);
            if (damageMultiplier < 0.5f) damageMultiplier = 0.5f;
        }
        
        CombatStats adjustedStats = stats;
        adjustedStats.attack *= damageMultiplier;
        
        float dealt = 0.f;
        bool crit = false;
        COMBAT->ApplyDamage(this, player, adjustedStats, victimStats, &dealt, &crit);
        
        // 피격 후 쿨다운 설정
        player->SetHitCooldown(1.0f);
        
        // 피격 사운드 재생
        player->PlayHitSound();
    }
}