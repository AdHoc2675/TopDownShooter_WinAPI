#pragma once
#include "CPanel.h"

class CPlayer;

enum class GameResult
{
    Victory,    // 보스 처치
    Defeat      // 플레이어 사망
};

class CResultPanel : public CPanel
{
public:
    CResultPanel();
    virtual ~CResultPanel();

    // 결과 화면 설정
    void Configure(GameResult result, float playTime, int level, int monstersKilled = 0, int reward = 0);

private:
    void Init() override;
    void OnEnable() override;
    void Update() override;
    void Render() override;
    void OnDisable() override;
    void Release() override;

    // 버튼 클릭 콜백
    static void OnRetryClicked(DWORD_PTR param1, DWORD_PTR param2);
    static void OnTitleClicked(DWORD_PTR param1, DWORD_PTR param2);
    static void OnExitClicked(DWORD_PTR param1, DWORD_PTR param2);  // 종료 버튼 추가

    GameResult result;
    float playTime;
    int playerLevel;
    int monstersKilled;
    int earnedReward = 0;
    bool buttonsCreated;
};