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

class CMemoryTile : public CTwoSideQuad
{
public:
    CMemoryTile(const glm::vec2 &leftTop, const glm::vec2 &size);

    void SetFrontFaced(bool value);

    void Update(float dt) override;
    void Draw()const override;

private:
    bool m_isFrontFaced = false;
    float m_rotation = 0;
};
