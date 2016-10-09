#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "libchapter3.h"

class IRenderer3D;

// Вершина с трёхмерной позицией, нормалью и 2D координатами текстуры.
struct SVertexP3NT2
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

// Массивы данных сетки вершин с трёхмерной позицией,
// нормалью и 2D коодинатами текстуры.
struct SMeshDataP3NT2
{
    std::vector<SVertexP3NT2> vertices;
    std::vector<uint32_t> indicies;
};

// Тип примитива, представленного сеткой.
enum class MeshType
{
    Triangles,
    TriangleFan,
    TriangleStrip,
};

// Класс для хранения в видеопамяти и рендеринга сетки вершин
// с трёхмерной позицией, нормалью и 2D коодинатами текстуры.
class CMeshP3NT2 : private boost::noncopyable
{
public:
    CMeshP3NT2(MeshType meshType);

    // Копирует данные в буфер в видеопамяти.
    void Copy(const SMeshDataP3NT2 &data);

    // Рисует сетку примитивов, используя массивы индексов и вершин.
    void Draw(IRenderer3D &renderer)const;

private:
    MeshType m_meshType;
    CBufferObject m_attributesBuffer;
    CBufferObject m_indexesBuffer;
    size_t m_verticiesCount = 0;
    size_t m_indiciesCount = 0;
};
