#include "pch.h"
#include "CResultPanel.h"
#include "CGame.h"
#include "CIconTextButton.h"

CResultPanel::CResultPanel()
{
    result = GameResult::Defeat;
    playTime = 0.f;
    playerLevel = 1;
    monstersKilled = 0;
    buttonsCreated = false;
    SetName(TEXT("ResultPanel"));
    SetScreenFixed(true);
}

CResultPanel::~CResultPanel()
{
}

void CResultPanel::Configure(GameResult res, float time, int level, int killed)
{
    result = res;
    playTime = time;
    playerLevel = level;
    monstersKilled = killed;
    buttonsCreated = false;
}

void CResultPanel::Init()
{
    const float panelW = 750.f;
    const float panelH = 450.f; 
    SetScale(Vec2(panelW, panelH));
    SetPos(Vec2(CGame::WINSIZE.x * 0.5f - panelW * 0.5f,
                CGame::WINSIZE.y * 0.5f - panelH * 0.5f));
}

void CResultPanel::OnEnable()
{
    if (buttonsCreated) return;
    buttonsCreated = true;

    const float btnW = 180.f;
    const float btnH = 50.f;
    const float btnY = 280.f;
    const float gap = 20.f;

    // 버튼 3개를 가로로 배치
    float totalWidth = btnW * 3.f + gap * 2.f;
    float startX = (scale.x - totalWidth) * 0.5f;

    //// 다시하기 버튼
    //auto* retryBtn = new CIconTextButton();
    //retryBtn->SetName(TEXT("RetryButton"));
    //retryBtn->SetPos(Vec2(startX, btnY));
    //retryBtn->SetScale(Vec2(btnW, btnH));
    //retryBtn->SetLabel(L"다시 시작");
    //retryBtn->SetLabelSize(18);
    //retryBtn->SetLabelColor(RGB(20, 20, 20));
    //retryBtn->SetClickCallback(&CResultPanel::OnRetryClicked, (DWORD_PTR)this, 0);
    //EVENT->AddChild(this, retryBtn);

    //// 타이틀 버튼
    //auto* titleBtn = new CIconTextButton();
    //titleBtn->SetName(TEXT("TitleButton"));
    //titleBtn->SetPos(Vec2(startX + btnW + gap, btnY));
    //titleBtn->SetScale(Vec2(btnW, btnH));
    //titleBtn->SetLabel(L"타이틀로");
    //titleBtn->SetLabelSize(18);
    //titleBtn->SetLabelColor(RGB(20, 20, 20));
    //titleBtn->SetClickCallback(&CResultPanel::OnTitleClicked, (DWORD_PTR)this, 0);
    //EVENT->AddChild(this, titleBtn);

    // 종료 버튼
    auto* exitBtn = new CIconTextButton();
    exitBtn->SetName(TEXT("ExitButton"));
    exitBtn->SetPos(Vec2(startX + btnW + gap, btnY));
    exitBtn->SetScale(Vec2(btnW, btnH));
    exitBtn->SetLabel(L"게임 종료");
    exitBtn->SetLabelSize(18);
    exitBtn->SetLabelColor(RGB(150, 30, 30));  // 빨간색 텍스트
    exitBtn->SetClickCallback(&CResultPanel::OnExitClicked, (DWORD_PTR)this, 0);
    EVENT->AddChild(this, exitBtn);
}

void CResultPanel::Update()
{
}

void CResultPanel::Render()
{
    // 반투명 어두운 배경 (전체 화면)
    RENDER->SetPen(PenType::Null, RGB(0, 0, 0), 0);
    RENDER->SetBrush(BrushType::Solid, RGB(0, 0, 0));

    // 패널 배경
    COLORREF bgColor = (result == GameResult::Victory) ? RGB(220, 240, 220) : RGB(240, 220, 220);
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 3);
    RENDER->SetBrush(BrushType::Solid, bgColor);
    RENDER->Rect(renderPos.x, renderPos.y, renderPos.x + scale.x, renderPos.y + scale.y);

    float cx = renderPos.x + scale.x * 0.5f;
    float y = renderPos.y + 40.f;

    // 결과 타이틀
    RENDER->SetTextBackMode(TextBackMode::Null);
    if (result == GameResult::Victory)
    {
        RENDER->SetText(36, RGB(50, 150, 50), TextAlign::Center);
        RENDER->Text(cx, y, L"승리!");
    }
    else
    {
        RENDER->SetText(36, RGB(180, 50, 50), TextAlign::Center);
        RENDER->Text(cx, y, L"패배...");
    }
    y += 60.f;

    // 결과 정보
    RENDER->SetText(20, RGB(0, 0, 0), TextAlign::Center);

    // 플레이 시간
    int minutes = (int)(playTime / 60.f);
    int seconds = (int)(playTime) % 60;
    wstring timeStr = L"플레이 시간: " + to_wstring(minutes) + L"분 " + to_wstring(seconds) + L"초";
    RENDER->Text(cx, y, timeStr);
    y += 35.f;

    // 레벨
    wstring levelStr = L"달성 레벨: " + to_wstring(playerLevel);
    RENDER->Text(cx, y, levelStr);
    y += 35.f;

    // 처치 수 (옵션)
    if (monstersKilled > 0)
    {
        wstring killStr = L"처치한 몬스터: " + to_wstring(monstersKilled);
        RENDER->Text(cx, y, killStr);
        y += 35.f;
    }

    // 결과 메시지
    y += 20.f;
    RENDER->SetText(16, RGB(80, 80, 80), TextAlign::Center);
    if (result == GameResult::Victory)
    {
        RENDER->Text(cx, y, L"보스를 처치했습니다!");
    }
    else
    {
        RENDER->Text(cx, y, L"다음에는 더 잘할 수 있을 거예요!");
    }
}

void CResultPanel::OnDisable()
{
}

void CResultPanel::Release()
{
}

void CResultPanel::OnRetryClicked(DWORD_PTR param1, DWORD_PTR param2)
{
    CResultPanel* panel = reinterpret_cast<CResultPanel*>(param1);
    if (panel && panel->GetScene())
    {
        panel->GetScene()->SetPaused(false);
        SOUND->Stop(TEXT("Wasteland Combat Loop"));
        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Stage01, 1.0f);
    }
}

void CResultPanel::OnTitleClicked(DWORD_PTR param1, DWORD_PTR param2)
{
    CResultPanel* panel = reinterpret_cast<CResultPanel*>(param1);
    if (panel && panel->GetScene())
    {
        panel->GetScene()->SetPaused(false);
        SOUND->Stop(TEXT("Wasteland Combat Loop"));
        CAMERA->FadeOut(0.5f);
        EVENT->ChangeScene(SceneType::Title, 1.0f);
    }
}

void CResultPanel::OnExitClicked(DWORD_PTR param1, DWORD_PTR param2)
{
    // 프로그램 종료
    PostQuitMessage(0);
}