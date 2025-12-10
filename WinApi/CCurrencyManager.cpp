#include "pch.h"
#include "CCurrencyManager.h"

CCurrencyManager* CCurrencyManager::instance = nullptr;

CCurrencyManager::CCurrencyManager()
{
    currency = 0;
    saveFilePath = L"";
}

CCurrencyManager::~CCurrencyManager()
{
}

CCurrencyManager* CCurrencyManager::GetInstance()
{
    if (instance == nullptr)
        instance = new CCurrencyManager();
    return instance;
}

void CCurrencyManager::DestroyInstance()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

void CCurrencyManager::Init()
{
    // 저장 경로 설정 (실행 파일과 같은 위치)
    saveFilePath = PATH + L"\\savedata.dat";
    Load();
}

void CCurrencyManager::Release()
{
    Save();
}

void CCurrencyManager::AddCurrency(int amount)
{
    if (amount > 0)
    {
        currency += amount;
        Save();
        Logger::Debug(L"[CCurrencyManager] Added " + std::to_wstring(amount) + 
            L", Total: " + std::to_wstring(currency));
    }
}

void CCurrencyManager::SpendCurrency(int amount)
{
    if (amount > 0 && currency >= amount)
    {
        currency -= amount;
        Save();
        Logger::Debug(L"[CCurrencyManager] Spent " + std::to_wstring(amount) + 
            L", Remaining: " + std::to_wstring(currency));
    }
}

void CCurrencyManager::Save()
{
    std::ofstream ofs(saveFilePath, std::ios::binary);
    if (ofs.is_open())
    {
        ofs.write(reinterpret_cast<const char*>(&currency), sizeof(currency));
        ofs.close();
        Logger::Debug(L"[CCurrencyManager] Saved currency: " + std::to_wstring(currency));
    }
    else
    {
        Logger::Debug(L"[CCurrencyManager] Failed to save currency");
    }
}

void CCurrencyManager::Load()
{
    std::ifstream ifs(saveFilePath, std::ios::binary);
    if (ifs.is_open())
    {
        ifs.read(reinterpret_cast<char*>(&currency), sizeof(currency));
        ifs.close();
        Logger::Debug(L"[CCurrencyManager] Loaded currency: " + std::to_wstring(currency));
    }
    else
    {
        currency = 0;
        Logger::Debug(L"[CCurrencyManager] No save file found, starting with 0");
    }
}

int CCurrencyManager::CalculateReward(int monstersKilled, float playTime, int level)
{
    // 죽인 적 * 3 + 시간(초) + 레벨 * 10
    int reward = (monstersKilled * 3) + static_cast<int>(playTime) + (level * 10);
    return reward;
}