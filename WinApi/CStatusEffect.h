#pragma once

enum class StatusEffectType
{
    Burn,       // 발화: 매 초 피해
    // 향후 확장 가능: Poison, Slow, Freeze 등
};

// 상태이상 최대 중첩 상수
constexpr int BURN_MAX_STACKS = 20;

struct StatusEffect
{
    StatusEffectType type;
    int stacks;           // 중첩 수
    int maxStacks;        // 최대 중첩 수
    float duration;       // 남은 지속 시간
    float tickTimer;      // 다음 틱까지 남은 시간
    float tickInterval;   // 틱 간격 (초)
    float damagePerStack; // 중첩당 피해량

    StatusEffect()
        : type(StatusEffectType::Burn)
        , stacks(1)
        , maxStacks(BURN_MAX_STACKS)
        , duration(5.0f)
        , tickTimer(1.0f)
        , tickInterval(1.0f)
        , damagePerStack(3.0f)
    {
    }
};