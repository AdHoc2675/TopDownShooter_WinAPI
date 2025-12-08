#pragma once
#include <vector>
#include <functional>

template<typename T>
class CObjectPool
{
public:
    CObjectPool(size_t initialSize = 20)
    {
        Expand(initialSize);
    }

    ~CObjectPool()
    {
        for (T* obj : pool)
            delete obj;
        for (T* obj : activeObjects)
            delete obj;
    }

    // 풀에서 오브젝트 가져오기
    T* Acquire()
    {
        T* obj = nullptr;
        
        if (pool.empty())
        {
            Expand(10); // 부족하면 10개 추가 생성
        }
        
        obj = pool.back();
        pool.pop_back();
        activeObjects.push_back(obj);
        
        return obj;
    }

    // 풀로 오브젝트 반환
    void Release(T* obj)
    {
        if (!obj) return;
        
        auto it = std::find(activeObjects.begin(), activeObjects.end(), obj);
        if (it != activeObjects.end())
        {
            activeObjects.erase(it);
            pool.push_back(obj);
        }
    }

    // 모든 활성 오브젝트 반환
    void ReleaseAll()
    {
        for (T* obj : activeObjects)
            pool.push_back(obj);
        activeObjects.clear();
    }

    size_t GetActiveCount() const { return activeObjects.size(); }
    size_t GetPooledCount() const { return pool.size(); }

private:
    void Expand(size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            pool.push_back(new T());
        }
    }

    std::vector<T*> pool;           // 대기 중인 오브젝트
    std::vector<T*> activeObjects;  // 활성화된 오브젝트
};