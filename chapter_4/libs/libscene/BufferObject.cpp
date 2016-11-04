#include "includes/opengl-common.hpp"
#include "BufferObject.h"
#include <stdexcept>

namespace
{
GLenum MapEnum(BufferType type)
{
    switch (type)
    {
    case BufferType::Attributes:
        return GL_ARRAY_BUFFER;
    case BufferType::Indicies:
        return GL_ELEMENT_ARRAY_BUFFER;
    default:
        throw std::logic_error("Unhandled VBO type");
    }
}

GLenum MapEnum(BufferUsage usage)
{
    switch (usage)
    {
    case BufferUsage::StaticDraw:
        return GL_STATIC_DRAW;
    case BufferUsage::DynamicDraw:
        return GL_DYNAMIC_DRAW;
    case BufferUsage::StreamDraw:
        return GL_STREAM_DRAW;
    default:
        throw std::logic_error("Unhandled VBO usage hint");
    }
}
}

CBufferObject::CBufferObject(BufferType type, BufferUsage usageHint)
    : m_bufferType(type)
    , m_usageHint(usageHint)
{
    glGenBuffers(1, &m_bufferId);
}

CBufferObject::~CBufferObject()
{
    glDeleteBuffers(1, &m_bufferId);
}

void CBufferObject::Unbind(BufferType type)
{
    const GLenum target = MapEnum(type);
    glBindBuffer(target, 0);
}

void CBufferObject::Bind()const
{
    const GLenum target = MapEnum(m_bufferType);
    glBindBuffer(target, m_bufferId);
}

void CBufferObject::Unbind()const
{
    Unbind(m_bufferType);
}

void CBufferObject::Copy(const void *data, unsigned byteCount)
{
    const GLenum target = MapEnum(m_bufferType);
    const GLenum usageHint = MapEnum(m_usageHint);
    glBindBuffer(target, m_bufferId);
    glBufferData(target, byteCount, data, usageHint);
}

unsigned CBufferObject::GetBufferSize() const
{
    const GLenum target = MapEnum(m_bufferType);
    GLint result = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &result);
    return unsigned(result);
}
