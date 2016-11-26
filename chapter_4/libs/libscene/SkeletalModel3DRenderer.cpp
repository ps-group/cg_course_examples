#include "SkeletalModel3DRenderer.h"
#include "Geometry.h"
#include "SkeletalModel3D.h"
#include "DrawUtils.h"
#include "libshading/ProgramUniform.h"
#include "libshading/VertexAttribute.h"
#include "libshading/IProgramAdapter.h"
#include "includes/glm-common.hpp"
#include "includes/opengl-common.hpp"
#include "AssimpUtils.h"
#include <iostream>

using glm::mat4;

void CSkeletalModel3DRenderer::SetWorldMat4(const glm::mat4 &value)
{
    m_world = value;
}

void CSkeletalModel3DRenderer::SetViewMat4(const glm::mat4 &value)
{
    m_view = value;
}

void CSkeletalModel3DRenderer::SetProjectionMat4(const glm::mat4 &value)
{
    m_projection = value;
}

void CSkeletalModel3DRenderer::Use(IProgramAdapter &program)
{
    m_pProgram = &program;
    m_pProgram->Use();
    GetUniform(UniformId::TEX_DIFFUSE) = 0; // GL_TEXTURE0
    GetUniform(UniformId::TEX_SPECULAR) = 1; // GL_TEXTURE1
    GetUniform(UniformId::TEX_EMISSIVE) = 2; // GL_TEXTURE2
}

void CSkeletalModel3DRenderer::Draw(CSkeletalModel3D &model)
{
    if (!m_pProgram)
    {
        throw std::runtime_error("Cannot draw 3D model while no program set");
    }
    SetupTransforms();

    // Обновляем трансформации костей по именам.
    UpdateNodeTransformsCache(model);

    model.m_pGeometry->Bind();
    for (CSkeletalMesh3D &mesh : model.m_meshes)
    {
        SetupBoneTransforms(mesh);
        ApplyMaterial(model.m_materials[mesh.m_materialIndex]);
        BindAttributes(mesh.m_layout);
        CDrawUtils::DrawRangeElements(mesh.m_layout);
    }
}

CProgramUniform CSkeletalModel3DRenderer::GetUniform(UniformId id) const
{
    return m_pProgram->GetUniform(id);
}

// Настраивает матрицы
//  - преобразования от коодинат сетки к коодинатам камеры,
//  - преобразования от мировых координат к координатам камеры,
//  - перспективного преобразования
void CSkeletalModel3DRenderer::SetupTransforms()
{
    const mat4 worldViewMat4 = m_view * m_world;
    const mat4 normalMat4 = CDrawUtils::GetNormalMat4(worldViewMat4);
    GetUniform(UniformId::MATRIX_PROJECTION) = m_projection;
    GetUniform(UniformId::MATRIX_VIEW) = m_view;
    GetUniform(UniformId::MATRIX_WORLDVIEW) = worldViewMat4;
    GetUniform(UniformId::MATRIX_NORMALWORLDVIEW) = normalMat4;
}

void CSkeletalModel3DRenderer::UpdateNodeTransformsCache(const CSkeletalModel3D &model)
{
    if (model.m_rootNode == nullptr)
    {
        throw std::runtime_error("Cannot render skeletal model with null root node");
    }

    const mat4 rootMat4 = model.m_rootNode->m_transform.ToMat4();
    const mat4 inverseRootMat4 = glm::inverse(rootMat4);

    m_nodeTransformsCache.clear();
    VisitNode(*model.m_rootNode, inverseRootMat4);
}

void CSkeletalModel3DRenderer::SetupBoneTransforms(const CSkeletalMesh3D &mesh)
{
    std::vector<mat4> data(Limits::MAX_BONES_COUNT);
    std::transform(mesh.m_bones.begin(), mesh.m_bones.end(),
                   data.begin(), [&](const CSkeletalBone &bone) {
        return m_nodeTransformsCache[bone.m_pNode->m_name]
                * bone.m_boneOffset;
    });

    GetUniform(UniformId::BONE_TRANSFORM_ARRAY) = data;
}

void CSkeletalModel3DRenderer::VisitNode(const CSkeletalNode &node,
                                         const glm::mat4 &parentMat4)
{
    // Преобразование вершины от координат узла (кости) к координатам модели,
    //  с учётом динамически изменяемой трансформации кости.
    const mat4 nodeMat4 = parentMat4 * node.m_transform.ToMat4();

    // Запоминаем трансформацию из системы коодинат модели
    //  в систему координат узла, описывающего кость.
    m_nodeTransformsCache[node.m_name] = nodeMat4;

    // Посещаем все дочерние узлы графа сцены данной модели.
    for (const auto &pChild : node.m_children)
    {
        VisitNode(*pChild, nodeMat4);
    }
}

// Применяет текстуры, цвет и shininess материала
//  к uniform-переменным шейдера и состоянию OpenGL.
void CSkeletalModel3DRenderer::ApplyMaterial(const SPhongMaterial &material) const
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
void CSkeletalModel3DRenderer::BindAttributes(const SGeometryLayout &layout) const
{
    auto bindFloat = [&](AttributeId attr, size_t offset, unsigned numComponents) {
        CVertexAttribute attrVar = m_pProgram->GetAttribute(attr);
        if (offset == SGeometryLayout::UNSET)
        {
            attrVar.DisablePointer();
        }
        else
        {
            const size_t bytesOffset = size_t(layout.m_baseVertexOffset + unsigned(offset));
            const size_t bytesStride = size_t(layout.m_vertexSize);
            attrVar.EnablePointer();
            attrVar.SetFloatsOffset(bytesOffset, bytesStride, numComponents, false);
        }
    };
    auto bindUint8 = [&](AttributeId attr, size_t offset, unsigned numComponents) {
        CVertexAttribute attrVar = m_pProgram->GetAttribute(attr);
        if (offset == SGeometryLayout::UNSET)
        {
            attrVar.DisablePointer();
        }
        else
        {
            const size_t bytesOffset = size_t(layout.m_baseVertexOffset + unsigned(offset));
            const size_t bytesStride = size_t(layout.m_vertexSize);
            attrVar.EnablePointer();
            attrVar.SetUint8Offset(bytesOffset, bytesStride, numComponents);
        }
    };
    bindFloat(AttributeId::POSITION, layout.m_position3D, 3);
    bindFloat(AttributeId::NORMAL, layout.m_normal, 3);
    bindFloat(AttributeId::TEX_COORD_UV, layout.m_texCoord2D, 2);
    bindUint8(AttributeId::BONE_INDICIES, layout.m_boneIndexes, 4);
    bindFloat(AttributeId::BONE_WEIGHTS, layout.m_boneWeights, 4);
}
