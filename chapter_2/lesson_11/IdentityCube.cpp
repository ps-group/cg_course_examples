#include "stdafx.h"
#include "IdentityCube.h"

#include <stdint.h>

namespace
{
struct SCubeFace
{
    uint16_t vertexIndex1;
    uint16_t vertexIndex2;
    uint16_t vertexIndex3;
    uint16_t vertexIndex4;
    uint16_t faceIndex;
};

// Вершины куба служат материалом для формирования треугольников,
// составляющих грани куба.
const glm::vec3 CUBE_VERTICIES[] = {
    {-1, +1, -1},
    {+1, +1, -1},
    {+1, -1, -1},
    {-1, -1, -1},
    {-1, +1, +1},
    {+1, +1, +1},
    {+1, -1, +1},
    {-1, -1, +1},
};

// Привыкаем использовать 16-битный unsigned short,
// чтобы экономить память на фигурах с тысячами вершин.
const SCubeFace CUBE_FACES[] = {
    {2, 3, 0, 1, static_cast<uint16_t>(CubeFace::Front)},
    {6, 2, 1, 5, static_cast<uint16_t>(CubeFace::Left)},
    {6, 7, 3, 2, static_cast<uint16_t>(CubeFace::Bottom)},
    {3, 7, 4, 0, static_cast<uint16_t>(CubeFace::Right)},
    {1, 0, 4, 5, static_cast<uint16_t>(CubeFace::Top)},
    {5, 4, 7, 6, static_cast<uint16_t>(CubeFace::Back)},
};
}

void CIdentityCube::Update(float)
{
    if (m_isDirty)
    {
        Triangulate();
        m_isDirty = false;
    }
}

void CIdentityCube::Draw() const
{
    m_mesh.Draw();
}

void CIdentityCube::SetFaceTextureRect(CubeFace face, const CFloatRect &rect)
{
    const size_t index = static_cast<size_t>(face);
    m_textureRects[index] = rect;
    m_isDirty = true;
}

void CIdentityCube::Triangulate()
{
    m_mesh.Clear(MeshType::Triangles);
    for (const SCubeFace &face : CUBE_FACES)
    {
        const CFloatRect texRect = m_textureRects[face.faceIndex];
        const glm::vec3 &coord1 = CUBE_VERTICIES[face.vertexIndex1];
        const glm::vec3 &coord2 = CUBE_VERTICIES[face.vertexIndex2];
        const glm::vec3 &coord3 = CUBE_VERTICIES[face.vertexIndex3];
        const glm::vec3 &coord4 = CUBE_VERTICIES[face.vertexIndex4];
        const glm::vec3 normal = glm::normalize(
                    glm::cross(coord2 - coord1, coord3 - coord1));

        const SVertexP3NT2 v1 = { coord1, texRect.GetTopLeft(), normal};
        const SVertexP3NT2 v2 = { coord2, texRect.GetTopRight(), normal};
        const SVertexP3NT2 v3 = { coord3, texRect.GetBottomRight(), normal};
        const SVertexP3NT2 v4 = { coord4, texRect.GetBottomLeft(), normal};

        const uint32_t fromIndex = uint32_t(m_mesh.m_vertices.size());
        m_mesh.m_vertices.push_back(v1);
        m_mesh.m_vertices.push_back(v2);
        m_mesh.m_vertices.push_back(v3);
        m_mesh.m_vertices.push_back(v4);
        m_mesh.m_indicies.push_back(fromIndex + 0);
        m_mesh.m_indicies.push_back(fromIndex + 1);
        m_mesh.m_indicies.push_back(fromIndex + 2);
        m_mesh.m_indicies.push_back(fromIndex + 0);
        m_mesh.m_indicies.push_back(fromIndex + 2);
        m_mesh.m_indicies.push_back(fromIndex + 3);
    }
}
