#pragma once

#include <functional>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// Вершина с трёхмерной позицией и нормалью.
struct SVertexP3N
{
    glm::vec3 position;
    glm::vec3 normal;
};

// Класс поверхности "Зонтик Уитни"
// https://en.wikipedia.org/wiki/Whitney_umbrella
class CWhitneyUmbrella
{
public:
    CWhitneyUmbrella(unsigned slices, unsigned stacks);

    void Draw()const;

private:
    void Tesselate(unsigned slices, unsigned stacks);

    std::vector<SVertexP3N> m_vertices;
    std::vector<uint32_t> m_indicies;
};
