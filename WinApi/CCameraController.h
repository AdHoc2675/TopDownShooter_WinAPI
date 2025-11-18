#pragma once

class CPlayer;   // 전방 선언

class CCameraController : public CGameObject
{
public:
    CCameraController();
    virtual ~CCameraController();

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;

public:
    void SetPlayer(CPlayer* player) { m_player = player; }

private:
    CPlayer* m_player = nullptr;
};
