#pragma once

#include "MemoryTile.h"
#include "Lights.h"

#define ENABLE_DEBUG_MEMORY_FIELD_HITS 0

class CMemoryField : public ISceneObject
{
public:
    CMemoryField();

    // ISceneObject interface
    void Update(float dt) final;
    void Draw() const final;

    void Activate(const CRay &ray);
    unsigned GetTileCount()const;
    unsigned GetTotalScore()const;

private:
    void GenerateTiles();
    void CheckTilesPair(std::pair<size_t, size_t> indicies);
    CFloatRect GetImageFrameRect(TileImage image)const;

    CPhongModelMaterial m_material;
    CTexture2DAtlas m_atlas;
    std::vector<CMemoryTile> m_tiles;
    unsigned m_totalScore = 0;

#if ENABLE_DEBUG_MEMORY_FIELD_HITS
    std::vector<glm::vec3> m_hits;
#endif
};
