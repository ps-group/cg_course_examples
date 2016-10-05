#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

enum class MeshType
{
    Triangles,
    TriangleFan,
    TriangleStrip,
};

// Вершина с трёхмерной позицией, нормалью и 2D координатами текстуры.
struct SVertexP3NT2
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

// Сетка вершин с трёхмерной позицией, нормалью и 2D коодинатами текстуры.
struct SMeshP3NT2
{
    std::vector<SVertexP3NT2> m_vertices;
    std::vector<uint32_t> m_indicies;
    MeshType m_meshType = MeshType::Triangles;

    // Очищает вершины и индексы, устанавливает тип примитива.
    void Clear(MeshType meshType);

    // Рисует сетку примитивов, используя массивы индексов и вершин.
    void Draw()const;
};
