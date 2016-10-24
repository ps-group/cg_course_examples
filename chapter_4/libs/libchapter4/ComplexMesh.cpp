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
