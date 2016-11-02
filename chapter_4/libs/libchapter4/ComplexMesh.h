#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "IMesh.h"
#include "MeshType.h"
#include "BufferObject.h"
#include "Texture2D.h"
#include "BoundingBox.h"

class CComplexMesh;
using CComplexMeshSharedPtr = std::shared_ptr<CComplexMesh>;

struct SSubMesh
{
    glm::mat4 m_transform;
    glm::uvec2 m_vertexRange;
    glm::uvec2 m_indexRange;
    MeshType m_type = MeshType::Triangles;
    unsigned m_materialIndex = 0;
    // Размер вершины и смещения атрибутов
    // от начала каждой вершины, в байтах.
    unsigned m_stride = 0;
    unsigned m_baseOffset = 0;
    int m_positionOffset = -1;
    int m_normalsOffset = -1;
    int m_textureOffset = -1;
    int m_tangentsOffset = -1;
};

struct SMaterial
{
    glm::vec4 m_diffuseColor;
    glm::vec4 m_specularColor;
    glm::vec4 m_emissiveColor;
    float m_shininess = 0;
    CTexture2DSharedPtr m_pDiffuse;
    CTexture2DSharedPtr m_pSpecular;
    CTexture2DSharedPtr m_pEmissive;
};

struct SComplexMeshData
{
    std::vector<SMaterial> m_materials;
    std::vector<SSubMesh> m_submeshes;
    std::vector<uint8_t> m_vertexData;
    std::vector<uint32_t> m_indicies;
    CBoundingBox m_bbox;
};

class CComplexMesh : public IMesh
{
public:
    CComplexMesh();

    void SetData(const SComplexMeshData &data);
    void SetData(SComplexMeshData &&data);

    void Draw(IMeshRenderer &renderer)const override;
    CBoundingBox GetBoundingBox()const override;

private:
    std::vector<SMaterial> m_materials;
    std::vector<SSubMesh> m_submeshes;
    CBufferObject m_verticies;
    CBufferObject m_indicies;
    CBoundingBox m_bbox;
};
