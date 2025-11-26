#pragma once
#include "CButton.h"

using namespace std;
class CImage;

class CIconTextButton : public CButton
{
public:
    CIconTextButton();
    virtual ~CIconTextButton();

    // 라벨 설정
    void SetLabel(const wstring& text)    { label = text; }
    void SetLabelColor(COLORREF c)             { labelColor = c; }
    void SetLabelSize(int s)                   { labelSize = s; }
    void SetLabelAlign(TextAlign a)            { labelAlign = a; }

    // 아이콘 설정
    void SetIcon(CImage* img)                  { icon = img; }
    void SetIconTransparent(COLORREF c)        { iconTransparent = c; }
    void SetIconPadding(const Vec2& p)         { iconPadding = p; }
    void SetIconPadding(float px, float py)    { iconPadding = Vec2(px, py); }
    void SetIconSize(const Vec2& s)            { iconSize = s; }
    void SetIconSize(float w, float h)         { iconSize = Vec2(w, h); }
    void UseAutoIconSize(bool b)               { autoIconSize = b; }

private:
    void Init()        override;
    void OnEnable()    override;
    void Update()      override;
    void Render()      override;
    void OnDisable()   override;
    void Release()     override;

private:
    // 표시 데이터
    wstring label;
    COLORREF     labelColor;
    int          labelSize;
    TextAlign    labelAlign;

    CImage*      icon;
    COLORREF     iconTransparent;
    Vec2         iconPadding;
    Vec2         iconSize;
    bool         autoIconSize;
};