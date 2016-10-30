#include "stdafx.h"
#include "MeshP3NT2.h"
#include "IRenderer3D.h"

namespace
{
GLenum GetPrimitiveType(MeshType type)
{
    switch (type)
    {
    case MeshType::Triangles:
        return GL_TRIANGLES;
    case MeshType::TriangleFan:
        return GL_TRIANGLE_FAN;
    case MeshType::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    default:
        throw std::logic_error("Unhandled mesh type");
    }
}
}

CMeshP3NT2::CMeshP3NT2(MeshType meshType)
    : m_meshType(meshType)
    , m_attributesBuffer(BufferType::Attributes)
    , m_indexesBuffer(BufferType::Indicies)
{
}

void CMeshP3NT2::Copy(const SMeshDataP3NT2 &data)
{
    m_indiciesCount = data.indicies.size();
    m_indexesBuffer.Copy(data.indicies);
    m_verticiesCount = data.vertices.size();
    m_attributesBuffer.Copy(data.vertices);
}

void CMeshP3NT2::Draw(IRenderer3D &renderer) const
{
    // Выполняем привязку vertex array, normal array, tex coord 2d array
    const size_t stride = sizeof(SVertexP3NT2);
    const size_t positionOffset = size_t(offsetof(SVertexP3NT2, position));
    const size_t normalOffset = size_t(offsetof(SVertexP3NT2, normal));
    const size_t texCoordOffset = size_t(offsetof(SVertexP3NT2, texCoord));

    m_attributesBuffer.Bind();
    m_indexesBuffer.Bind();

    renderer.SetPosition3DOffset(positionOffset, stride);
    renderer.SetNormalOffset(normalOffset, stride);
    renderer.SetTexCoord2DOffset(texCoordOffset, stride);

    const GLenum primitive = GetPrimitiveType(m_meshType);
    const GLvoid *indexOffset = reinterpret_cast<const GLvoid *>(0);
    const GLuint minIndex = 0;
    const GLuint maxIndex = GLuint(m_verticiesCount);
    const GLsizei size = GLsizei(m_indiciesCount);

    glDrawRangeElements(primitive, minIndex, maxIndex, size,
                   GL_UNSIGNED_INT, indexOffset);
}
