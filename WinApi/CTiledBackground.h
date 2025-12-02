#pragma once
class CTiledBackground : public CGameObject
{
public:
    CTiledBackground();
    ~CTiledBackground() override = default;

private:
    void Init() override {}
    void OnEnable() override {}
    void Update() override {}
    void Render() override;
    void OnDisable() override {}
    void Release() override {}

private:

};