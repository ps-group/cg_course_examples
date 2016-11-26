#include "includes/opengl-common.hpp"
#include "ArrayObject.h"

CArrayObject::CArrayObject()
{
    glGenVertexArrays(1, &m_arrayId);
}

CArrayObject::CArrayObject(do_bind_tag)
{
    glGenVertexArrays(1, &m_arrayId);
    Bind();
}

CArrayObject::~CArrayObject()
{
    glDeleteVertexArrays(1, &m_arrayId);
}

void CArrayObject::Bind()
{
    glBindVertexArray(m_arrayId);
}
