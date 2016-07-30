#include "stdafx.h"
#include "RevolutionBodies.h"
#include <algorithm>

namespace
{
const unsigned MIN_PRECISION = 4;

void CalculateTriangleStripIndicies(std::vector<uint32_t> &indicies,
                                    unsigned columnCount, unsigned rowCount)
{
    indicies.clear();
    indicies.reserve((columnCount - 1) * rowCount * 2);
    // вычисляем индексы вершин.
    for (unsigned ci = 0; ci < columnCount - 1; ++ci)
    {
        if (ci % 2 == 0)
        {
            for (unsigned ri = 0; ri < rowCount; ++ri)
            {
                unsigned index = ci * rowCount + ri;
                indicies.push_back(index + rowCount);
                indicies.push_back(index);
            }
        }
        else
        {
            for (unsigned ri = rowCount - 1; ri < rowCount; --ri)
            {
                unsigned index = ci * rowCount + ri;
                indicies.push_back(index);
                indicies.push_back(index + rowCount);
            }
        }
    }
}

/// Привязывает вершины к состоянию OpenGL,
/// затем вызывает 'callback'.
template <class T>
void DoWithBindedArrays(const std::vector<SVertexP3N> &vertices, T && callback)
{
    // Включаем режим vertex array и normal array.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Выполняем привязку vertex array и normal array
    const size_t stride = sizeof(SVertexP3N);
    glNormalPointer(GL_FLOAT, stride, glm::value_ptr(vertices[0].normal));
    glVertexPointer(3, GL_FLOAT, stride, glm::value_ptr(vertices[0].position));

    // Выполняем внешнюю функцию.
    callback();

    // Выключаем режим vertex array и normal array.
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
}

CIdentitySphere::CIdentitySphere(unsigned slices, unsigned stacks)
{
    Tesselate(slices, stacks);
}

void CIdentitySphere::Draw() const
{
    DoWithBindedArrays(m_vertices, [this] {
        glDrawElements(GL_TRIANGLE_STRIP, GLsizei(m_indicies.size()),
                       GL_UNSIGNED_INT, m_indicies.data());
    });
}

void CIdentitySphere::Tesselate(unsigned slices, unsigned stacks)
{
    assert((slices >= MIN_PRECISION) && (stacks >= MIN_PRECISION));
    m_vertices.reserve(slices * stacks);
    // вычисляем позиции вершин.
    for (unsigned ci = 0; ci < slices; ++ci)
    {
        const float u = float(ci) / float(slices - 2);
        for (unsigned ri = 0; ri < stacks; ++ri)
        {
            const float v = float(ri) / float(stacks - 1);
            m_vertices.push_back(SVertexP3N(GetPosition(u, v)));
        }
    }

    // Нормаль к сфере - это нормализованный вектор радиуса к данной точке
    // Поскольку координаты центра равны 0, координаты вектора радиуса
    // будут равны координатам вершины.
    // Благодаря радиусу, равному 1, нормализация не требуется.
    for (SVertexP3N &v : m_vertices)
    {
        v.normal = v.position;
    }
    CalculateTriangleStripIndicies(m_indicies, slices, stacks);
}

glm::vec3 CIdentitySphere::GetPosition(float u, float v) const
{
    const float pi2 = float(2.0 * M_PI);
    const float radius = 1.f;
    const float latitude = pi2 * (u - 0.5f); // 𝟐𝝅∙(𝒖−𝟎.𝟓)
    const float longitude = pi2 * v; // 𝟐𝝅∙𝒗
    const float latitudeRadius = radius * sinf(fabsf(latitude));

    return { cosf(longitude) * latitudeRadius,
             cosf(latitude) * radius,
             sinf(longitude) * latitudeRadius };
}
