#include "stdafx.h"
#include "PlanetRenderer.h"
#include "PlanetProgram.h"
#include "libscene/DrawUtils.h"
#include "includes/opengl-common.hpp"


CPlanetRenderer3D::CPlanetRenderer3D(const IProgramAdapter &program)
    : m_program(program)
{
    m_program.Use();
    GetUniform(UniformId::TEX_DIFFUSE) = 0; // GL_TEXTURE0
    GetUniform(UniformId::TEX_SPECULAR) = 1; // GL_TEXTURE1
    GetUniform(UniformId::TEX_EMISSIVE) = 2; // GL_TEXTURE2
}

CPlanetRenderer3D::~CPlanetRenderer3D()
{
}

void CPlanetRenderer3D::SetupLight0(const glm::vec4 &position, const glm::vec4 &diffuse, const glm::vec4 &specular)
{
    GetUniform(UniformId::LIGHT_POSITION) = position;
    GetUniform(UniformId::LIGHT_DIFFUSE) = diffuse;
    GetUniform(UniformId::LIGHT_SPECULAR) = specular;
}

void CPlanetRenderer3D::SetWorldMat4(const glm::mat4 &value)
{
    m_world = value;
}

void CPlanetRenderer3D::SetViewMat4(const glm::mat4 &value)
{
    m_view = value;
}

void CPlanetRenderer3D::SetProjectionMat4(const glm::mat4 &value)
{
    m_projection = value;
}

void CPlanetRenderer3D::Draw(const CMeshComponent &mesh)
{
    const glm::mat4 modelView = m_view * m_world;
    GetUniform(UniformId::MATRIX_PROJECTION) = m_projection;
    GetUniform(UniformId::MATRIX_VIEW) = m_view;
    GetUniform(UniformId::MATRIX_WORLDVIEW) = modelView;
    GetUniform(UniformId::MATRIX_NORMALWORLDVIEW)
            = CDrawUtils::GetNormalMat4(modelView);

    mesh.m_geometry.m_pGeometry->Bind();
    BindTextures(mesh);
    BindAttributes(mesh.m_geometry.m_layout);
    CDrawUtils::DrawRangeElements(mesh.m_geometry.m_layout);
}

CProgramUniform CPlanetRenderer3D::GetUniform(UniformId id) const
{
    return m_program.GetUniform(id);
}

void CPlanetRenderer3D::BindTextures(const CMeshComponent &mesh)
{
    // Привязываем текстуры к разным текстурным слотам,
    //  ранее установленным для uniform-переменных (см. `Use`).
    glActiveTexture(GL_TEXTURE2);
    CDrawUtils::MaybeBind(mesh.m_pEmissive);
    glActiveTexture(GL_TEXTURE1);
    CDrawUtils::MaybeBind(mesh.m_pSpecular);
    glActiveTexture(GL_TEXTURE0);
    CDrawUtils::MaybeBind(mesh.m_pDiffuse);
    // После окончания привязки активной должна остаться GL_TEXTURE0
}

void CPlanetRenderer3D::BindAttributes(const SGeometryLayout &layout) const
{
    CVertexAttribute position = m_program.GetAttribute(AttributeId::POSITION);
    position.EnablePointer();
    position.SetVec3Offset(layout.m_position3D, layout.m_vertexSize, false);

    CVertexAttribute normal = m_program.GetAttribute(AttributeId::NORMAL);
    normal.EnablePointer();
    normal.SetVec3Offset(layout.m_normal, layout.m_vertexSize, false);

    CVertexAttribute texCoord = m_program.GetAttribute(AttributeId::TEX_COORD_UV);
    texCoord.EnablePointer();
    texCoord.SetVec2Offset(layout.m_texCoord2D, layout.m_vertexSize);
}
