#pragma once
#include "CCollider.h"

#define MAXLAYER 32

class CCollisionManager : public SingleTon<CCollisionManager>
{
    friend SingleTon<CCollisionManager>;
private:
    CCollisionManager();
    virtual ~CCollisionManager();

public:
    void Init();
    void Update();
    void Release();

    void AddCollider(CCollider* collider);
    void RemoveCollider(CCollider* collider);

    void CheckLayer(UINT left, UINT right);
    void UnCheckLayer(UINT left, UINT right);
    void ResetCheck();

    // 오브젝트 풀링용 - 충돌 이력 정리
    void ClearColliderHistory(CCollider* collider);

private:
    void CollisionUpdate(UINT left, UINT right);
    UINT64 CollisionID(UINT leftID, UINT rightID);

private:
    array<array<bool, MAXLAYER>, MAXLAYER> layerMask;
    array<list<CCollider*>, MAXLAYER> colliderList;
    unordered_map<UINT64, bool> prevCollision;
};

