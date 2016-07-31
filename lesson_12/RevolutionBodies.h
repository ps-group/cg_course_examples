#pragma once

#include "IBody.h"
#include <functional>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using Function2D = std::function<float(float, float)>;

// Вершина с трёхмерной позицией и нормалью.
struct SVertexP3NT2
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

class CIdentitySphere final : public IBody
{
public:
    CIdentitySphere(unsigned slices, unsigned stacks);

    void Update(float) final {}
    void Draw()const final;

private:
    void Tesselate(unsigned slices, unsigned stacks);
    glm::vec3 GetPosition(float u, float v)const;

    std::vector<SVertexP3NT2> m_vertices;
    std::vector<uint32_t> m_indicies;
};
