#include "stdafx.h"
#include "EarthRenderer3D.h"
#include "EarthProgramContext.h"


CEarthRenderer3D::CEarthRenderer3D(CEarthProgramContext &context)
    : m_context(context)
    , m_vertexAttr(m_context.GetPositionAttr())
    , m_normalAttr(m_context.GetNormalAttr())
    , m_texCoordAttr(m_context.GetTexCoordAttr())
{
    m_context.Use();
    m_vertexAttr.EnablePointer();
    m_normalAttr.EnablePointer();
    m_texCoordAttr.EnablePointer();
}

CEarthRenderer3D::~CEarthRenderer3D()
{
    m_vertexAttr.DisablePointer();
    m_normalAttr.DisablePointer();
    m_texCoordAttr.DisablePointer();
}

void CEarthRenderer3D::SetTexCoord2DOffset(size_t offset, size_t stride)
{
    m_texCoordAttr.SetVec2Offset(offset, stride);
}

void CEarthRenderer3D::SetPosition3DOffset(size_t offset, size_t stride)
{
    m_vertexAttr.SetVec3Offset(offset, stride, false);
}

void CEarthRenderer3D::SetNormalOffset(size_t offset, size_t stride)
{
    m_normalAttr.SetVec3Offset(offset, stride, false);
}
