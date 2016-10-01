#pragma once
#include "libchapter2.h"
#include <vector>

// Вершина с двумерной позицией, нормалью и 2D координатами текстуры.
struct SVertexP3NT2
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

// Прямоугольный двусторонний спрайт, лежащий в плоскости Oxz.
// Передняя сторона имеет нормаль (0, +1, 0),
// Задняя сторона имеет нормаль (0, -1, 0).
class CTwoSideQuad : public ISceneObject
{
public:
    CTwoSideQuad(const glm::vec2 &leftTop, const glm::vec2 &size);

    void Update(float) override {}
    void Draw()const override;

    void SetFrontTextureRect(const CFloatRect &rect);
    void SetBackTextureRect(const CFloatRect &rect);

private:
    std::vector<SVertexP3NT2> m_vertices;
    std::vector<uint8_t> m_indicies;
};

enum class TileImage
{
    FLY = 0,
    EXIT_SIGN,
    HEART,
    KEY,
    MUSHROOM,
    SNAIL,
    SPRINGBOARD,
    FISH,

    NUM_TILE_IMAGES,
};

class IMemoryTileController
{
public:
    virtual ~IMemoryTileController() = default;
    virtual void OnTileAnimationStarted() = 0;
    virtual void OnTileAnimationEnded() = 0;
};

class CMemoryTile : public CTwoSideQuad
{
public:
    using VoidHandler = std::function<void()>;

    CMemoryTile(IMemoryTileController &controller, TileImage tileImage,
                const glm::vec2 &leftTop, const glm::vec2 &size);

    TileImage GetTileImage() const;
    void SetTileImage(TileImage GetTileImage);

    bool IsFrontFaced()const;
    bool IsAlive()const;

    bool MaybeActivate(const glm::vec2 &point);
    void Deactivate();
    void Kill();

    void Update(float dt) override;
    void Draw()const override;

private:
    void SetAnimationActive(bool value);

    std::reference_wrapper<IMemoryTileController> m_controllerRef;
    TileImage m_tileImage = TileImage::FLY;
    CFloatRect m_bounds;
    bool m_isFrontFaced = false;
    float m_rotation = 0;
    bool m_isAnimationActive = false;
    bool m_isAlive = true;
};
