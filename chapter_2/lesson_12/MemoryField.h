#pragma once

#include "MemoryTile.h"
#include "Lights.h"

class CMemoryField : public ISceneObject
{
public:
    CMemoryField();

    // ISceneObject interface
public:
    void Update(float dt) final;
    void Draw() const final;

private:
    void GenerateTiles();

    CPhongModelMaterial m_material;
    CTexture2DUniquePtr m_pTileBackTexture;
    std::vector<CMemoryTile> m_tiles;
};
