#include "libchapter4_private.h"
#include "ComplexMesh.h"

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

void ApplyMaterial(IComplexMeshRenderer &renderer, const SMaterial &mat)
{
    // TODO: добавить обработку цветов, заменяющих или изменяющих текстуры.
    renderer.BindTexture(IComplexMeshRenderer::Diffuse, mat.m_pDiffuse.get());
    renderer.BindTexture(IComplexMeshRenderer::Specular, mat.m_pSpecular.get());
    renderer.BindTexture(IComplexMeshRenderer::Emissive, mat.m_pEmissive.get());
}

void CComplexMesh::Draw(IComplexMeshRenderer &renderer)
{
    m_verticies.Bind();
    m_indicies.Bind();
    for (const SSubMesh &submesh : m_submeshes)
    {
        ApplyMaterial(renderer, m_materials[submesh.m_materialIndex]);

        auto applyAttribute = [&](IComplexMeshRenderer::Attribute attr, int offset) {
            if (offset >= 0)
            {
                renderer.BindAttribute(attr, size_t(offset), size_t(submesh.m_stride));
            }
            else
            {
                renderer.UnbindAttribute(attr);
            }
        };
        applyAttribute(IComplexMeshRenderer::Position3D, submesh.m_positionOffset);
        applyAttribute(IComplexMeshRenderer::Normal, submesh.m_normalsOffset);
        applyAttribute(IComplexMeshRenderer::TexCoord2D, submesh.m_textureOffset);

        const GLuint start = submesh.m_vertexRange.x;
        const GLuint end = submesh.m_vertexRange.y;
        const GLsizei count = GLsizei(submesh.m_indexRange.y - submesh.m_indexRange.x + 1);
        const GLsizei offset = GLsizei(submesh.m_indexRange.x);
        glDrawRangeElements(GL_TRIANGLES, start, end, count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(offset));
    }
}
