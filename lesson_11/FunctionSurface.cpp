#include "stdafx.h"
#include "FunctionSurface.h"
#include <algorithm>

namespace
{
const float DOT_SIZE = 5.f;

glm::vec3 GetPosition(const Function2D &fn, float x, float z)
{
    const float y = fn(x, z);
    return {x, y, z};
}

// вычисляет нормали численным методом,
// с помощью векторного произведения.
void CalculateNormals(std::vector<SVertexP3N> &vertices,
                      const Function2D &fn, float step)
{
    for (SVertexP3N &v : vertices)
    {
        const glm::vec3 &position = v.position;
        glm::vec3 dir1 = GetPosition(fn, position.x, position.z + step) - position;
        glm::vec3 dir2 = GetPosition(fn, position.x + step, position.z) - position;
        v.normal = glm::normalize(glm::cross(dir1, dir2));
    }
}

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

CDottedFunctionSurface::CDottedFunctionSurface(const Function2D &fn)
    : m_fn(fn)
{
}

void CDottedFunctionSurface::Setup(const glm::vec2 &rangeX, const glm::vec2 &rangeZ, float step)
{
    m_vertices.clear();
    // вычисляем позиции вершин.
    for (float x = rangeX.x; x < rangeX.y; x += step)
    {
        for (float z = rangeZ.x; z < rangeZ.y; z += step)
        {
            m_vertices.push_back(SVertexP3N(GetPosition(m_fn, x, z)));
        }
    }
    CalculateNormals(m_vertices, m_fn, step);
}

void CDottedFunctionSurface::Draw() const
{
    glPointSize(DOT_SIZE);
    DoWithBindedArrays(m_vertices, [this] {
        glDrawArrays(GL_POINTS, 0, GLsizei(m_vertices.size()));
    });
}

CSolidFunctionSurface::CSolidFunctionSurface(const Function2D &fn)
    : m_fn(fn)
{
}

void CSolidFunctionSurface::Setup(const glm::vec2 &rangeX, const glm::vec2 &rangeZ, float step)
{
    m_vertices.clear();
    const unsigned columnCount = unsigned((rangeX.y - rangeX.x) / step);
    const unsigned rowCount = unsigned((rangeZ.y - rangeZ.x) / step);

    // вычисляем позиции вершин.
    for (unsigned ci = 0; ci < columnCount; ++ci)
    {
        const float x = rangeX.x + step * float(ci);
        for (unsigned ri = 0; ri < rowCount; ++ri)
        {
            const float z = rangeZ.x + step * float(ri);
            m_vertices.push_back(SVertexP3N(GetPosition(m_fn, x, z)));
        }
    }
    CalculateNormals(m_vertices, m_fn, step);
    // вычисляем индексы вершин.
    for (unsigned ci = 0; ci < columnCount - 1; ++ci)
    {
        if (ci % 2 == 0)
        {
            for (unsigned ri = 0; ri < rowCount; ++ri)
            {
                unsigned index = ci * rowCount + ri;
                m_indicies.push_back(index + rowCount);
                m_indicies.push_back(index);
            }
        }
        else
        {
            for (unsigned ri = rowCount - 1; ri < rowCount; --ri)
            {
                unsigned index = ci * rowCount + ri;
                m_indicies.push_back(index);
                m_indicies.push_back(index + rowCount);
            }
        }
    }
}

void CSolidFunctionSurface::Draw() const
{
    DoWithBindedArrays(m_vertices, [this] {
        glDrawElements(GL_TRIANGLE_STRIP, GLsizei(m_indicies.size()),
                       GL_UNSIGNED_INT, m_indicies.data());
    });
}
