#pragma once

#include <functional>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// Вершина с трёхмерной позицией, нормалью и 2D координатами текстуры.
struct SVertexP3NT2
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

class CIdentitySphere
{
public:
    CIdentitySphere(unsigned slices, unsigned stacks);

    void Draw()const;

private:
    void Tesselate(unsigned slices, unsigned stacks);

    std::vector<SVertexP3NT2> m_vertices;
    std::vector<uint32_t> m_indicies;
};
