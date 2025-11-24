#pragma once

class CPlayer;   // 전방 선언
class CImage;

class CCameraController : public CGameObject
{
public:
    CCameraController();
    virtual ~CCameraController();
    void SetPlayer(CPlayer* player) { m_player = player; }

private:
    void Init()         override;
    void OnEnable()     override;
    void Update()       override;
    void Render()       override;
    void OnDisable()    override;
    void Release()      override;
 
    CPlayer* m_player = nullptr;
	CImage* m_cursorImage = nullptr;
};
