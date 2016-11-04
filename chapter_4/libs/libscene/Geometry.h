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

    // Координаты вершины.
    size_t m_position3D = UNSET;
    // Координаты наложения тестур.
    size_t m_texCoord2D = UNSET;
    // Нормаль к касательной плоскости.
    size_t m_normal = UNSET;
    // Тангенциальный вектор касательной плоскости.
    size_t m_tangent = UNSET;
    // Битангенциальный вектор касательной плоскости,
    //  ортогонален плоскости, заданной m_normal и m_tangent
    size_t m_bitangent = UNSET;
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
