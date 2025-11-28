#pragma once
class CTiledBackground : public CGameObject
{
public:
    CTiledBackground();
    ~CTiledBackground() override = default;

    // 기본 설정
    void AddTile(CImage* img)                { if (img) tiles.push_back(img); }
    void SetTileSize(int w, int h)           { tileW = w; tileH = h; }
    void SetMargin(float m)                  { margin = m; }

    // 단색 배경 + 산발적 데코 모드
    void SetBackgroundColor(COLORREF c)      { backColor = c; }
    void EnableSparseDecor(bool enable)      { sparseDecor = enable; }
    void SetDecorCellSize(int w, int h)      { decorCellW = w; decorCellH = h; }
    void SetDecorDensity(float d)            { decorDensity = d; } // 0~1 (확률)

private:
    void Init() override {}
    void OnEnable() override {}
    void Update() override {}
    void Render() override;
    void OnDisable() override {}
    void Release() override {}

    // 산발적 배치용 해시 -> 0~1 확률 판정
    float Hash01(int x, int y) const;
    int   PickDecorIndex(int cellX, int cellY) const;

private:
    std::vector<CImage*> tiles;

    int      tileW      = 32;
    int      tileH      = 32;
    float    margin     = 64.f;

    // 단색 + 희소 데코 관련
    COLORREF backColor      = RGB(39, 32, 48); // 기본 어두운 회색
    bool     sparseDecor    = false;
    int      decorCellW     = 256;             // 한 셀 크기
    int      decorCellH     = 256;
    float    decorDensity   = 0.5f;           // 셀 당 배치 확률(0.1=10%)
};