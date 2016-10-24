#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "MeshType.h"
#include "BufferObject.h"
#include "Texture2D.h"
#include "BoundingBox.h"

struct SSubMesh
{
    glm::uvec2 m_vertexRange;
    glm::uvec2 m_indexRange;
    MeshType m_type = MeshType::Triangles;
    bool m_hasTexCoord: 1;
    bool m_hasTangentSpace: 1;
    unsigned m_materialIndex = 0;
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
    std::vector<float> m_vertexData;
    std::vector<uint32_t> m_indicies;
    CBoundingBox m_bbox;
};

class CComplexMesh
{
public:
    CComplexMesh();

    void SetData(const SComplexMeshData &data);
    void SetData(SComplexMeshData &&data);

private:
    std::vector<SMaterial> m_materials;
    std::vector<SSubMesh> m_submeshes;
    CBufferObject m_verticies;
    CBufferObject m_indicies;
    CBoundingBox m_bbox;
};
