#include "stdafx.h"
#include "MemoryField.h"

namespace
{
const char TILE_BACK_TEXTURE[] = "res/memory-trainer/tile-back.png";
const char TILE_FRONT_ATLAS[] = "res/memory-trainer/spites.plist";

const unsigned FIELD_WIDTH = 6;
const unsigned FIELD_HEIGHT = 5;
const float TILE_SIZE = 0.9f;
const float TILE_MARGIN = 0.05f;
}

CMemoryField::CMemoryField()
{
    GenerateTiles();

    // Load tile texture.
    CTexture2DLoader loader;
    loader.SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
    m_pTileBackTexture = loader.Load(TILE_BACK_TEXTURE);

    // Setup material.
    const float MATERIAL_SHININESS = 30.f;
    const glm::vec4 GRAY_RGBA = {0.3f, 0.3f, 0.3f, 1.f};
    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
    m_material.SetAmbient(WHITE_RGBA);
    m_material.SetDiffuse(WHITE_RGBA);
    m_material.SetSpecular(GRAY_RGBA);
    m_material.SetShininess(MATERIAL_SHININESS);
}

void CMemoryField::Update(float dt)
{
    for (auto &tile : m_tiles)
    {
        tile.Update(dt);
    }
}

void CMemoryField::Draw() const
{
    m_material.Setup();
    m_pTileBackTexture->DoWhileBinded([&] {
        for (const auto &tile : m_tiles)
        {
            tile.Draw();
        }
    });
}

void CMemoryField::GenerateTiles()
{
    static_assert(FIELD_WIDTH * FIELD_HEIGHT % 2 == 0, "Tiles count must be even");

    const float step = TILE_SIZE + TILE_MARGIN;
    const float leftmostX = float(-0.5f * step * FIELD_WIDTH);
    const float topmostY = float(-0.5f * step * FIELD_HEIGHT);
    for (unsigned row = 0; row < FIELD_HEIGHT; ++row)
    {
        const float top = float(topmostY + row * step);
        for (unsigned column = 0; column < FIELD_WIDTH; ++column)
        {
            const float left = float(leftmostX + column * step);
            m_tiles.emplace_back(glm::vec2{left, top},
                                 glm::vec2{TILE_SIZE, TILE_SIZE});

            CFloatRect identity({0, 0}, {1, 1});
            m_tiles.back().SetFrontTextureRect(CFloatRect(identity));
            m_tiles.back().SetBackTextureRect(CFloatRect(identity));
        }
    }
}
