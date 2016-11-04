#include "Model3DRenderer.h"
#include "Geometry.h"
#include "Model3D.h"
#include "DrawUtils.h"
#include "libshading/ProgramUniform.h"
#include "libshading/VertexAttribute.h"
#include "libshading/IProgramAdapter.h"
#include "includes/glm-common.hpp"
#include "includes/opengl-common.hpp"


void CModel3DRenderer::SetWorldMat4(const glm::mat4 &value)
{
    m_world = value;
}

void CModel3DRenderer::SetViewMat4(const glm::mat4 &value)
{
    m_view = value;
}

void CModel3DRenderer::SetProjectionMat4(const glm::mat4 &value)
{
    m_projection = value;
}

void CModel3DRenderer::Use(IProgramAdapter &program)
{
    m_pProgram = &program;
    m_pProgram->Use();
    GetUniform(UniformId::TEX_DIFFUSE) = 0; // GL_TEXTURE0
    GetUniform(UniformId::TEX_SPECULAR) = 1; // GL_TEXTURE1
    GetUniform(UniformId::TEX_EMISSIVE) = 2; // GL_TEXTURE2
}

void CModel3DRenderer::Draw(CModel3D &model)
{
    if (!m_pProgram)
    {
        throw std::runtime_error("Cannot draw 3D model while no program set");
    }
    GetUniform(UniformId::MATRIX_PROJECTION) = m_projection;
    GetUniform(UniformId::MATRIX_VIEW) = m_view;

    model.m_pGeometry->Bind();
    for (CMesh3D &mesh : model.m_meshes)
    {
        ApplyModelView(mesh.m_local);
        ApplyMaterial(model.m_materials[mesh.m_materialIndex]);
        BindAttributes(mesh.m_layout);
        CDrawUtils::DrawRangeElements(mesh.m_layout);
    }
}

CProgramUniform CModel3DRenderer::GetUniform(UniformId id) const
{
    return m_pProgram->GetUniform(id);
}

// Получает матрицу преобразования к локальным координатам модели
//  и настраивает матрицы преобразования
//  от коодинат сетки к коодинатам камеры.
void CModel3DRenderer::ApplyModelView(const glm::mat4 &local)
{
    const glm::mat4 worldMatrix = m_view * m_world * local;
    const glm::mat4 normalMatrix = CDrawUtils::GetNormalMatrix(worldMatrix);
    GetUniform(UniformId::MATRIX_WORLDVIEW) = worldMatrix;
    GetUniform(UniformId::MATRIX_NORMALWORLDVIEW) = normalMatrix;
}

// Применяет текстуры, цвет и shininess материала
//  к uniform-переменным шейдера и состоянию OpenGL.
void CModel3DRenderer::ApplyMaterial(const SPhongMaterial &material) const
{
    GetUniform(UniformId::MATERIAL_SHININESS) = material.shininess;
    GetUniform(UniformId::MATERIAL_DIFFUSE) = material.diffuseColor;
    GetUniform(UniformId::MATERIAL_SPECULAR) = material.specularColor;
    GetUniform(UniformId::MATERIAL_EMISSIVE) = material.emissiveColor;

    // Привязываем текстуры к разным текстурным слотам,
    //  ранее установленным для uniform-переменных (см. `Use`).
    glActiveTexture(GL_TEXTURE2);
    CDrawUtils::MaybeBind(material.pEmissive);
    glActiveTexture(GL_TEXTURE1);
    CDrawUtils::MaybeBind(material.pSpecular);
    glActiveTexture(GL_TEXTURE0);
    CDrawUtils::MaybeBind(material.pDiffuse);
    // После окончания привязки активной должна остаться GL_TEXTURE0
}

// Выполняет привязку смещений и параметров атрибутов вершин
//  к атрибутным переменным шейдера.
void CModel3DRenderer::BindAttributes(const SGeometryLayout &layout) const
{
    auto bind = [&](AttributeId attr, size_t offset, unsigned numComponents, bool needClamp) {
        CVertexAttribute attrVar = m_pProgram->GetAttribute(attr);
        if (attrVar.IsValid())
        {
            if (offset == SGeometryLayout::UNSET)
            {
                attrVar.DisablePointer();
            }
            else
            {
                const size_t bytesOffset = size_t(layout.m_baseVertexOffset + unsigned(offset));
                const size_t bytesStride = size_t(layout.m_vertexSize);
                attrVar.EnablePointer();
                attrVar.SetOffset(bytesOffset, bytesStride, numComponents, needClamp);
            }
        }
    };
    bind(AttributeId::POSITION, layout.m_position3D, 3, false);
    bind(AttributeId::NORMAL, layout.m_normal, 3, false);
    bind(AttributeId::TANGENT, layout.m_tangent, 3, false);
    bind(AttributeId::BITANGENT, layout.m_bitangent, 3, false);
    bind(AttributeId::TEX_COORD_UV, layout.m_texCoord2D, 2, true);
}
