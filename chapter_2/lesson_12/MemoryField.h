#pragma once

#include "MemoryTile.h"
#include "Lights.h"

#define ENABLE_DEBUG_MEMORY_FIELD_HITS 0

class CMemoryField : public ISceneObject
{
public:
    CMemoryField();

    // ISceneObject interface
public:
    void Update(float dt) final;
    void Draw() const final;

    void Activate(const CRay &ray);

private:
    void GenerateTiles();

    CPhongModelMaterial m_material;
    CTexture2DUniquePtr m_pTileBackTexture;
    std::vector<CMemoryTile> m_tiles;

#if ENABLE_DEBUG_MEMORY_FIELD_HITS
    std::vector<glm::vec3> m_hits;
#endif
};
