#include "includes/opengl-common.hpp"
#include "VertexAttribute.h"

CVertexAttribute::CVertexAttribute(int location)
    : m_location(location)
{
}

bool CVertexAttribute::IsValid() const
{
    return (m_location != -1);
}

void CVertexAttribute::EnablePointer()
{
    glEnableVertexAttribArray(GLuint(m_location));
}

void CVertexAttribute::DisablePointer()
{
    glDisableVertexAttribArray(GLuint(m_location));
}

void CVertexAttribute::SetVec3Offset(size_t offset, size_t stride, bool needClamp)
{
    const GLboolean normalize = needClamp ? GL_TRUE : GL_FALSE;
    glVertexAttribPointer(GLuint(m_location), 3, GL_FLOAT, normalize,
                          GLsizei(stride), reinterpret_cast<const void *>(offset));
}

void CVertexAttribute::SetVec2Offset(size_t offset, size_t stride)
{
    const GLboolean normalize = GL_FALSE;
    glVertexAttribPointer(GLuint(m_location), 2, GL_FLOAT, normalize,
                          GLsizei(stride), reinterpret_cast<const void *>(offset));
}

void CVertexAttribute::SetOffset(size_t offset, size_t stride, unsigned numComponents, bool needClamp)
{
    const GLboolean normalize = needClamp ? GL_TRUE : GL_FALSE;
    glVertexAttribPointer(GLuint(m_location), GLint(numComponents), GL_FLOAT, normalize,
                          GLsizei(stride), reinterpret_cast<const void *>(offset));
}
