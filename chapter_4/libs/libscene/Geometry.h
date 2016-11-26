#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "../libgeometry/BoundingBox.h"
#include "Texture2D.h"
#include "BufferObject.h"

// Тип примитива, представленного сеткой.
enum class PrimitiveType
{
    Points,
    Lines,
    Triangles,
    TriangleFan,
    TriangleStrip,
};

namespace Limits
{
// Максимальное число костей, от которых зависит вершина
static const size_t BONES_PER_VERTEX = 4;

// Максимальное число костей в одной модели.
static const size_t MAX_BONES_COUNT = 64;
}

// Гибкая шаблонная структура для хранения вершинных данных,
//  индесов вершин и ограничивающего параллелипипеда.
template <class TV, class TI>
class SGeometryData
{
public:
    CBoundingBox m_bbox;
    std::vector<TV> m_vertexData;
    std::vector<TI> m_indicies;
};

// Структура для хранения размещения вершин
//  в памяти целого буфера видепамяти либо его участка.
// Все размеры заданы в байтах.
struct SGeometryLayout
{
    // Специальное значение, означающее, что атрибут отсутствует.
    static const size_t UNSET = size_t(-1);

    // Тип хранимых примитивов.
    PrimitiveType m_primitive = PrimitiveType::Points;

    // Количество вершин и индексов
    size_t m_vertexCount = 0;
    size_t m_indexCount = 0;

    // Размер вершины и базовое смещение всех атрибутов / индексов.
    size_t m_vertexSize = 0;
    size_t m_baseVertexOffset = 0;
    size_t m_baseIndexOffset = 0;

    // Смещение координат вершины, тип данных vec3.
    size_t m_position3D = UNSET;
    // Смещение координат наложения тестур, тип данных vec2.
    size_t m_texCoord2D = UNSET;
    // Смещение нормали к касательной плоскости, тип данных vec3.
    size_t m_normal = UNSET;
    // Смещение тангенциального вектора касательной плоскости, тип данных vec3.
    size_t m_tangent = UNSET;
    // Смещение битангенциального вектора касательной плоскости,
    //  ортогонального плоскости, заданной нормалью и тангенциальным вектором,
    //  тип данных vec3
    size_t m_bitangent = UNSET;
    // Смещение индексов используемых костей, тип данных uint8_t[4]
    size_t m_boneIndexes = UNSET;
    // Смещение весов используемых костей, тип данных vec4,
    //  если вершина зависит менее чем от 4 костей, ненужные кости
    //  будут иметь нулевой вес.
    size_t m_boneWeights = UNSET;
};

class CGeometry : private boost::noncopyable
{
public:
    CGeometry()
        : m_verticies(BufferType::Attributes)
        , m_indicies(BufferType::Indicies)
    {
    }

    template<class TV, class TI>
    void Copy(const SGeometryData<TV, TI> &data)
    {
        m_verticies.Copy(data.m_vertexData);
        m_indicies.Copy(data.m_indicies);
        m_bbox = data.m_bbox;
    }

    void Bind()
    {
        m_indicies.Bind();
        m_verticies.Bind();
    }

private:
    CBufferObject m_verticies;
    CBufferObject m_indicies;
    CBoundingBox m_bbox;
};

using CGeometrySharedPtr = std::shared_ptr<CGeometry>;

// Материал модели освещения Фонга.
struct SPhongMaterial
{
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    glm::vec4 emissiveColor;
    float shininess = 0;
    CTexture2DSharedPtr pDiffuse;
    CTexture2DSharedPtr pSpecular;
    CTexture2DSharedPtr pEmissive;
};
