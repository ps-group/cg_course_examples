#pragma once
#include <glm/vec2.hpp>
#include <vector>

// Вершина с двумерной позицией и 2D координатами текстуры.
struct SVertexP2T2
{
    glm::vec2 position;
    glm::vec2 texCoord;
};

class CQuad
{
public:
    CQuad(const glm::vec2 &leftTop, const glm::vec2 &size);

    void Draw()const;

private:
    std::vector<SVertexP2T2> m_vertices;
    std::vector<uint8_t> m_indicies;
};
