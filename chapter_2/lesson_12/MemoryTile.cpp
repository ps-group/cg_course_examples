#include "stdafx.h"
#include "MemoryTile.h"

namespace
{
// Скорость вращения, радианов в секунду.
const float TILE_ROTATION_SPEED = 1.f;

/// Привязывает вершины к состоянию OpenGL,
/// затем вызывает 'callback'.
template <class T>
void DoWithBindedArrays(const std::vector<SVertexP3NT2> &vertices, T && callback)
{
    // Включаем режимы привязки нужных данных.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Выполняем привязку vertex array, normal array, texture array.
    const size_t stride = sizeof(SVertexP3NT2);
    glVertexPointer(3, GL_FLOAT, stride, glm::value_ptr(vertices[0].position));
    glNormalPointer(GL_FLOAT, stride, glm::value_ptr(vertices[0].normal));
    glTexCoordPointer(2, GL_FLOAT, stride, glm::value_ptr(vertices[0].texCoord));

    // Выполняем внешнюю функцию.
    callback();

    // Выключаем режимы привязки данных.
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

SVertexP3NT2 MakeVertex(const glm::vec2 &xz, float normalY)
{
    SVertexP3NT2 vertex;
    vertex.position = { xz.x, 0.f, xz.y };
    vertex.normal = { 0.f, normalY, 0.f };
    return vertex;
}
}

CTwoSideQuad::CTwoSideQuad(const glm::vec2 &leftTop, const glm::vec2 &size)
{
    SVertexP3NT2 vLeftTopFront = MakeVertex(leftTop, 1.f);
    SVertexP3NT2 vRightTopFront = MakeVertex(leftTop + glm::vec2{ size.x, 0.f }, 1.f);
    SVertexP3NT2 vLeftBottomFront = MakeVertex(leftTop + glm::vec2{ 0.f, size.y }, 1.f);
    SVertexP3NT2 vRightBottomFront = MakeVertex(leftTop + glm::vec2{ size.x, size.y }, 1.f);

    SVertexP3NT2 vLeftTopBack = MakeVertex(leftTop, -1.f);
    SVertexP3NT2 vRightTopBack = MakeVertex(leftTop + glm::vec2{ size.x, 0.f }, -1.f);
    SVertexP3NT2 vLeftBottomBack = MakeVertex(leftTop + glm::vec2{ 0.f, size.y }, -1.f);
    SVertexP3NT2 vRightBottomBack = MakeVertex(leftTop + glm::vec2{ size.x, size.y }, -1.f);

    m_vertices = { vLeftTopFront, vRightTopFront, vLeftBottomFront, vRightBottomFront,
                   vLeftTopBack, vRightTopBack, vLeftBottomBack, vRightBottomBack };
    m_indicies = { 0, 1, 2, 1, 3, 2,
                   6, 5, 4, 6, 7, 5 };
}

void CTwoSideQuad::Draw() const
{
    DoWithBindedArrays(m_vertices, [this] {
        glDrawElements(GL_TRIANGLES, GLsizei(m_indicies.size()),
                       GL_UNSIGNED_BYTE, m_indicies.data());
    });
}

void CTwoSideQuad::SetFrontTextureRect(const CFloatRect &rect)
{
    m_vertices[0].texCoord = rect.GetTopLeft();
    m_vertices[1].texCoord = rect.GetTopRight();
    m_vertices[2].texCoord = rect.GetBottomLeft();
    m_vertices[3].texCoord = rect.GetBottomRight();
}

void CTwoSideQuad::SetBackTextureRect(const CFloatRect &rect)
{
    m_vertices[4].texCoord = rect.GetTopLeft();
    m_vertices[5].texCoord = rect.GetTopRight();
    m_vertices[6].texCoord = rect.GetBottomLeft();
    m_vertices[7].texCoord = rect.GetBottomRight();
}

CMemoryTile::CMemoryTile(const glm::vec2 &leftTop, const glm::vec2 &size)
    : CTwoSideQuad(leftTop, size)
{
}

void CMemoryTile::SetFrontFaced(bool value)
{
    m_isFrontFaced = value;
}

void CMemoryTile::Update(float dt)
{
    const float expectedRotation = m_isFrontFaced ? float(M_PI) : 0.f;
    const float rotationDelta = TILE_ROTATION_SPEED * dt;

    if ((fabs(m_rotation - expectedRotation) < rotationDelta))
    {
        m_rotation = expectedRotation;
    }
    else if (m_rotation < expectedRotation)
    {
        m_rotation += rotationDelta;
    }
    else
    {
        m_rotation += -rotationDelta;
    }
}

void CMemoryTile::Draw() const
{
    const glm::vec3 zAxis = {0, 0, 1};
    const glm::mat4 transform = glm::rotate(glm::mat4(), m_rotation, zAxis);

    glPushMatrix();
    glMultMatrixf(glm::value_ptr(transform));
    CTwoSideQuad::Draw();
    glPopMatrix();
}
