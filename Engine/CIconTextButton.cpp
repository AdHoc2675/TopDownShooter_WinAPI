#include "pch.h"
#include "CIconTextButton.h"
#include "CImage.h"

CIconTextButton::CIconTextButton()
{
    label.clear();
    labelColor    = RGB(20, 20, 20);
    labelSize     = 12;
    labelAlign    = TextAlign::Left;

    icon          = nullptr;
    iconTransparent = RGB(255, 0, 255); // 마젠타 투명
    iconPadding   = Vec2(10.f, 10.f);
    iconSize      = Vec2(0.f, 0.f);     // 0이면 자동
    autoIconSize  = true;
}

CIconTextButton::~CIconTextButton()
{
}

void CIconTextButton::Init()
{
}

void CIconTextButton::OnEnable()
{
}

void CIconTextButton::Update()
{
    // 상태 계산이 필요하면 여기서 가능(현재는 렌더에서 실시간 계산)
}

void CIconTextButton::Render()
{
    // 호버/프레스 상태(기반 클래스 변경 없이 계산)
    const bool hovered = IsMouseOn();
    const bool pressed = hovered && INPUT->ButtonStay(VK_LBUTTON, true);

    // 배경/테두리
    COLORREF fill = pressed ? RGB(230, 230, 230)
                            : (hovered ? RGB(245, 245, 245) : RGB(255, 255, 255));
    RENDER->SetPen(PenType::Solid, RGB(0, 0, 0), 1);
    RENDER->SetBrush(BrushType::Solid, fill);
    RENDER->Rect(renderPos.x, renderPos.y, renderPos.x + scale.x, renderPos.y + scale.y);

    // 아이콘
    float textStartX = renderPos.x + iconPadding.x;
    if (icon)
    {
        float iw = iconSize.x;
        float ih = iconSize.y;
        if (autoIconSize || iw <= 0.f || ih <= 0.f)
        {
            ih = scale.y - iconPadding.y * 2.f;
            if (ih < 0.f) ih = 0.f;
            iw = ih; // 정사각 아이콘
        }

        const float x0 = renderPos.x + iconPadding.x;
        const float y0 = renderPos.y + (scale.y - ih) * 0.5f;
        const float x1 = x0 + iw;
        const float y1 = y0 + ih;

        RENDER->TransparentImage(icon, x0, y0, x1, y1, iconTransparent);

        textStartX = x0 + iw + 8.f; // 아이콘 뒤 여백
    }

    // 라벨
    if (!label.empty())
    {
        const float textY = renderPos.y + (scale.y * 0.5f) - (float)labelSize * 0.5f;
        RENDER->SetText(labelSize, labelColor, TextAlign::Left);
        RENDER->SetTextBackMode(TextBackMode::Null);
        RENDER->Text(textStartX, textY, label);
    }
}

void CIconTextButton::OnDisable()
{
}

void CIconTextButton::Release()
{
}