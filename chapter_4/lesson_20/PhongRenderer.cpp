#include "stdafx.h"
#include "PhongRenderer.h"
#include "PhongProgram.h"


CPlanetRenderer3D::CPlanetRenderer3D(CPlanetProgram &context)
    : m_context(context)
    , m_vertexAttr(m_context.GetPositionAttr())
    , m_normalAttr(m_context.GetNormalAttr())
    , m_texCoordAttr(m_context.GetTexCoordAttr())
{
    m_context.Use();
//    m_vertexAttr.EnablePointer();
//    m_normalAttr.EnablePointer();
//    m_texCoordAttr.EnablePointer();
}

CPlanetRenderer3D::~CPlanetRenderer3D()
{
    m_vertexAttr.DisablePointer();
    m_normalAttr.DisablePointer();
    m_texCoordAttr.DisablePointer();
}

void CPlanetRenderer3D::BindAttribute(Attribute attribute, size_t offset, size_t stride)
{
    switch (attribute)
    {
    case Position3D:
        m_vertexAttr.EnablePointer();
        m_vertexAttr.SetVec3Offset(offset, stride, false);
        break;
    case Normal:
        m_normalAttr.EnablePointer();
        m_normalAttr.SetVec3Offset(offset, stride, false);
        break;
    case TexCoord2D:
        m_texCoordAttr.EnablePointer();
        m_texCoordAttr.SetVec2Offset(offset, stride);
        break;
    }
}

void CPlanetRenderer3D::UnbindAttribute(Attribute attribute)
{
    switch (attribute)
    {
    case Position3D:
        m_vertexAttr.DisablePointer();
        break;
    case Normal:
        m_normalAttr.DisablePointer();
        break;
    case TexCoord2D:
        m_texCoordAttr.DisablePointer();
        break;
    }
}

void CPlanetRenderer3D::BindTexture(Layer layer, CTexture2D *pTexture)
{
    CTexture2D black(CTexture2D::no_texture_tag{});
    switch (layer)
    {
    case Diffuse:
        m_context.BindDiffuseMap(pTexture ? *pTexture : black);
        break;
    case Specular:
        m_context.BindSpecularMap(pTexture ? *pTexture : black);
        break;
    case Emissive:
        m_context.BindEmissiveMap(pTexture ? *pTexture : black);
        break;
    }
}
