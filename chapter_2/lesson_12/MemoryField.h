#pragma once

#include "MemoryTile.h"
#include "Lights.h"

#define ENABLE_DEBUG_MEMORY_FIELD_HITS 0

class CMemoryField : public ISceneObject, public IMemoryTileController
{
public:
    CMemoryField();

    // ISceneObject interface
public:
    void Update(float dt) final;
    void Draw() const final;

    void Activate(const CRay &ray);

    // IMemoryTileController interface
private:
    void OnTileAnimationStarted() final;
    void OnTileAnimationEnded() final;

private:
    void GenerateTiles();
    void CheckTilesPair(std::pair<size_t, size_t> indicies);
    CFloatRect GetImageFrameRect(TileImage image)const;

    CPhongModelMaterial m_material;
    CTexture2DAtlas m_atlas;
    std::vector<CMemoryTile> m_tiles;
    bool m_isActivateDisabled = false;

#if ENABLE_DEBUG_MEMORY_FIELD_HITS
    std::vector<glm::vec3> m_hits;
#endif
};
