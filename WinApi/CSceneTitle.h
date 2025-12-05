#pragma once
#include "CScene.h"

class CSceneTitle : public CScene
{
public:
    CSceneTitle();
    virtual ~CSceneTitle();

    void Init()   override;
    void Enter()  override;
    void Update() override;
    void Render() override;
    void Exit()   override;
    void Release() override;

private:
    int selected = 0; // 0=Pistol, 1=Shotgun
};

