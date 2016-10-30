#include "stdafx.h"
#include "PhongRenderer.h"
#include "PhongProgram.h"


CPlanetRenderer3D::CPlanetRenderer3D(CPlanetProgram &program)
    : m_program(program)
    , m_vertexAttr(m_program.GetPositionAttr())
    , m_normalAttr(m_program.GetNormalAttr())
    , m_texCoordAttr(m_program.GetTexCoordAttr())
{
    m_program.Use();
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

void CPlanetRenderer3D::SetWorldTransform(const glm::mat4 &value)
{
    m_worldTransform = value;
}

void CPlanetRenderer3D::SetTransform(const glm::mat4 &transform)
{
    m_program.SetModel(m_worldTransform * transform);
    m_program.UpdateModelViewProjection();
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

void CPlanetRenderer3D::SetMaterialLayer(Layer layer, CTexture2D *pTexture, const glm::vec4 &color)
{
    switch (layer)
    {
    case Diffuse:
        m_program.BindDiffuseMap(pTexture, color);
        break;
    case Specular:
        m_program.BindSpecularMap(pTexture, color);
        break;
    case Emissive:
        m_program.BindEmissiveMap(pTexture, color);
        break;
    }
}
