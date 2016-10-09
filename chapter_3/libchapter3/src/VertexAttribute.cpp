#include "libchapter3_private.h"
#include "VertexAttribute.h"

CVertexAttribute::CVertexAttribute(int location)
    : m_location(location)
{
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
