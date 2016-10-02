#pragma once
#include "libchapter2.h"

struct SVertexP2T2
{
    glm::vec2 position;
    glm::vec2 texCoord;
};

// Класс представляет двумерный текстурированный спрайт,
//  обладающий размером, положением и точкой крепления
//  спрайта к своему положению (она задаётся в диапазоне [0..1]).
// По умолчанию точка крепления - левый верхний угол,
//  то есть положение спрайта определяет положение
//  его левого верхнего угла.
class CSprite : public ISceneObject
{
public:
    void Update(float dt) final;
    void Draw() const final;

    glm::vec2 GetPosition()const;
    glm::vec2 GetSize() const;
    const CTexture2D &GetTexture() const;
    bool HasTexture()const;

    void SetPosition(const glm::vec2 &position);
    void SetSize(const glm::vec2 &size);
    void SetAnchorPoint(const glm::vec2 &point);
    void SetTexture(CTexture2DUniquePtr &&pTexture);

private:
    void Tesselate();

    std::vector<SVertexP2T2> m_vertices;
    std::vector<uint8_t> m_indicies;
    CTexture2DUniquePtr m_pTexture;
    glm::vec2 m_position;
    glm::vec2 m_size;
    glm::vec2 m_anchorPoint;
    bool m_didTesselate = false;
};

class CHeadUpDisplay : public ISceneObject
{
public:
    using GetWindowSizeFn = std::function<glm::ivec2()>;

    CHeadUpDisplay(const GetWindowSizeFn &getWindowSize);

    void SetScore(int value);
    void SetTilesLeft(int value);

    void Update(float dt)final;
    void Draw()const final;

private:
    struct SIntegerIndicator
    {
        CSprite sprite;
        int value = 0;
        bool didRasterize = false;
    };

    CTexture2DUniquePtr RasterizeText(const std::string &text) const;
    void UpdateIndicator(SIntegerIndicator &indicator, const char *textPrefix)const;

    GetWindowSizeFn m_getWindowSize;
    TTFFontPtr m_pFont;
    SIntegerIndicator m_score;
    SIntegerIndicator m_tilesLeft;
};
