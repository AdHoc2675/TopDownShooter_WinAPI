#pragma once
#include <string>
#include <fstream>

class CCurrencyManager
{
private:
    CCurrencyManager();
    ~CCurrencyManager();

    static CCurrencyManager* instance;

public:
    static CCurrencyManager* GetInstance();
    static void DestroyInstance();

    void Init();
    void Release();

    // 재화 관련
    int  GetCurrency() const { return currency; }
    void AddCurrency(int amount);
    void SpendCurrency(int amount);
    bool CanAfford(int amount) const { return currency >= amount; }

    // 파일 저장/로드
    void Save();
    void Load();

    // 보상 계산: 죽인 적 * 3 + 시간(초) + 레벨 * 10
    static int CalculateReward(int monstersKilled, float playTime, int level);

private:
    int currency = 0;
    std::wstring saveFilePath;
};

#define CURRENCY CCurrencyManager::GetInstance()