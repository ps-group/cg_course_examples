#include "libchapter4_private.h"
#include "ComplexMesh.h"

namespace
{
void ApplyMaterial(IMeshRenderer &renderer, const SMaterial &mat)
{
    // Если shininess меньше единицы, мы считаем его неверно заданным
    //  и заменяем на значение по-умолчанию.
    const float DEFAULT_SHININESS = 10;
    const float shininess = mat.m_shininess < 1.f ? DEFAULT_SHININESS : mat.m_shininess;

    renderer.ApplyShininess(shininess);
    renderer.SetMaterialLayer(MaterialLayer::Diffuse, mat.m_pDiffuse.get(), mat.m_diffuseColor);
    renderer.SetMaterialLayer(MaterialLayer::Specular, mat.m_pSpecular.get(), mat.m_specularColor);
    renderer.SetMaterialLayer(MaterialLayer::Emissive, mat.m_pEmissive.get(), mat.m_emissiveColor);
}
}

CComplexMesh::CComplexMesh()
    : m_verticies(BufferType::Attributes, BufferUsage::StaticDraw)
    , m_indicies(BufferType::Indicies, BufferUsage::StaticDraw)
{
}

void CComplexMesh::SetData(const SComplexMeshData &data)
{
    m_materials = data.m_materials;
    m_submeshes = data.m_submeshes;
    m_verticies.Copy(data.m_vertexData);
    m_indicies.Copy(data.m_indicies);
    m_bbox = data.m_bbox;
}

void CComplexMesh::SetData(SComplexMeshData &&data)
{
    m_materials = std::move(data.m_materials);
    m_submeshes = std::move(data.m_submeshes);
    m_verticies.Copy(data.m_vertexData);
    m_indicies.Copy(data.m_indicies);
    m_bbox = std::move(data.m_bbox);
}

CBoundingBox CComplexMesh::GetBoundingBox() const
{
    return m_bbox;
}

void CComplexMesh::Draw(IMeshRenderer &renderer) const
{
    m_verticies.Bind();
    m_indicies.Bind();
    for (const SSubMesh &submesh : m_submeshes)
    {
        renderer.SetTransform(submesh.m_transform);
        ApplyMaterial(renderer, m_materials[submesh.m_materialIndex]);

        auto applyAttribute = [&](VertexAttribute attr, int offset) {
            if (offset >= 0)
            {
                const size_t bytesOffset = size_t(submesh.m_baseOffset + unsigned(offset));
                const size_t bytesStide = size_t(submesh.m_stride);
                renderer.BindAttribute(attr, bytesOffset, bytesStide);
            }
            else
            {
                renderer.UnbindAttribute(attr);
            }
        };
        applyAttribute(VertexAttribute::Position3D, submesh.m_positionOffset);
        applyAttribute(VertexAttribute::Normal, submesh.m_normalsOffset);
        applyAttribute(VertexAttribute::TexCoord2D, submesh.m_textureOffset);

        const GLuint start = submesh.m_vertexRange.x;
        const GLuint end = submesh.m_vertexRange.y;
        const GLsizei count = GLsizei(submesh.m_indexRange.y - submesh.m_indexRange.x + 1);
        const GLsizei offset = GLsizei(sizeof(uint32_t) * submesh.m_indexRange.x);
        glDrawRangeElements(GL_TRIANGLES, start, end, count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(offset));
    }
}
