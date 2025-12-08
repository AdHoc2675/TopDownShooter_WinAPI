#pragma once
#include "CObjectPool.h"
#include "CMonster.h"
#include "CRangedMonster.h"
#include "CSuicideBomberMonster.h"

class CMonsterPoolManager : public SingleTon<CMonsterPoolManager>
{
public:
    CMonsterPoolManager() = default;
    ~CMonsterPoolManager() = default;

    void Init(size_t normalCount = 30, size_t rangedCount = 10, size_t bomberCount = 10)
    {
        normalPool = new CObjectPool<CMonster>(normalCount);
        rangedPool = new CObjectPool<CRangedMonster>(rangedCount);
        bomberPool = new CObjectPool<CSuicideBomberMonster>(bomberCount);
    }

    void Release()
    {
        delete normalPool;
        delete rangedPool;
        delete bomberPool;
        normalPool = nullptr;
        rangedPool = nullptr;
        bomberPool = nullptr;
    }

    // 일반 몬스터 획득
    CMonster* AcquireNormal()
    {
        CMonster* m = normalPool->Acquire();
        m->Reset();
        m->SetPooled(true);
        return m;
    }

    // 원거리 몬스터 획득
    CRangedMonster* AcquireRanged()
    {
        CRangedMonster* m = rangedPool->Acquire();
        m->Reset();
        m->SetPooled(true);
        return m;
    }

    // 자폭 몬스터 획득
    CSuicideBomberMonster* AcquireBomber()
    {
        CSuicideBomberMonster* m = bomberPool->Acquire();
        m->Reset();
        m->SetPooled(true);
        return m;
    }

    // 풀로 반환
    void ReleaseMonster(CMonster* m)
    {
        if (!m || !m->IsPooled()) return;
        
        if (auto* bomber = dynamic_cast<CSuicideBomberMonster*>(m))
            bomberPool->Release(bomber);
        else if (auto* ranged = dynamic_cast<CRangedMonster*>(m))
            rangedPool->Release(ranged);
        else
            normalPool->Release(m);
    }

    // 모든 몬스터 반환 (씬 종료 시)
    void ReleaseAll()
    {
        if (normalPool) normalPool->ReleaseAll();
        if (rangedPool) rangedPool->ReleaseAll();
        if (bomberPool) bomberPool->ReleaseAll();
    }

private:
    CObjectPool<CMonster>* normalPool = nullptr;
    CObjectPool<CRangedMonster>* rangedPool = nullptr;
    CObjectPool<CSuicideBomberMonster>* bomberPool = nullptr;
};

#define MONSTERPOOL CMonsterPoolManager::GetInstance()