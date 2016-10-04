#include "stdafx.h"
#include "MemoryField.h"
#include <random>

namespace
{
const char TILE_MAP_ATLAS[] = "res/memory-trainer/spites.plist";

const unsigned FIELD_WIDTH = 6;
const unsigned FIELD_HEIGHT = 5;
const float TILE_SIZE = 0.9f;
const float TILE_MARGIN = 0.05f;

// Класс генерирует изображения для плиток
//  на основе линейного распределения вероятностей.
class CRandomTileImageGenerator
{
    using linear_random_int = std::uniform_int_distribution<int>;

public:
    CRandomTileImageGenerator()
    {
        std::random_device rd;
        m_random.seed(rd());

        const int max = static_cast<int>(TileImage::NUM_TILE_IMAGES) - 1;
        m_tileImageRange.param(linear_random_int::param_type(0, max));
    }


    // Генерирует массив изображений для плиток,
    //  при этом количество плиток разных типов всегда чётное.
    void GenerateRandomImagesRange(std::vector<TileImage> &range, size_t count)
    {
        // Проверяем, что число плиток чётное.
        assert(count % 2 == 0);
        range.resize(count);

        // Генерируем типы изображений, при этом обеспечиваем
        //  чётное количество изображений каждого типа
        //  за счёт зеркалирования значений
        for (size_t i = 0; i < count / 2; ++i)
        {
            TileImage image = GenerateTileImage();
            range.at(i) = image;
            range.at(count - i - 1) = image;
        }

        // Перемешиваем случайным образом, чтобы устранить
        //  последствия зеркалирования изображений.
        const int SHUFFLE_TIMES = 3;
        for (int i = 0; i < SHUFFLE_TIMES; ++i)
            std::shuffle(range.begin(), range.end(), m_random);
    }

private:
    TileImage GenerateTileImage()
    {
        return static_cast<TileImage>(m_tileImageRange(m_random));
    }

    linear_random_int m_tileImageRange;
    std::mt19937 m_random;
};
}

CMemoryField::CMemoryField()
    : m_atlas(TILE_MAP_ATLAS)
{
    GenerateTiles();

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

    // Ищем индексы плиток, повёрнутых лицевой частью.
    std::vector<size_t> indicies;
    indicies.reserve(2);
    for (size_t i = 0; i < m_tiles.size(); ++i)
    {
        if (m_tiles[i].IsFrontFaced())
        {
            indicies.push_back(i);
        }
    }

    // Если повернуты ровно две плитки,
    //  проверяем их по правилам игры.
    if (indicies.size() == 2)
    {
        CheckTilesPair({ indicies.front(), indicies.back() });
    }

    // Применяем идиому "remove-erase", чтобы удалить отмершие плитки.
    auto newEnd = std::remove_if(m_tiles.begin(), m_tiles.end(), [](const auto &tile) {
        return !tile.IsAlive();
    });
    m_tiles.erase(newEnd, m_tiles.end());
}

void CMemoryField::Draw() const
{
    m_material.Setup();
    m_atlas.GetTexture().DoWhileBinded([&] {
        for (const auto &tile : m_tiles)
        {
            tile.Draw();
        }
    });

    // В целях отладки можно рисовать все точки, в которых было
    //  пересечение
#if ENABLE_DEBUG_MEMORY_FIELD_HITS
    glPointSize(10.f);
    glBegin(GL_POINTS);
    for (const glm::vec3 &point : m_hits)
    {
        glVertex3fv(glm::value_ptr(point));
    }
    glEnd();
#endif
}

void CMemoryField::Activate(const CRay &ray)
{
    // Опираемся на соглашение, по которому
    //  все спрайты лежат в плоскости Oxz.
    CPlane plane({1, 0, 1}, {1, 0, 0}, {0, 0, 1});
    SRayIntersection intersection;
    if (!plane.Hit(ray, intersection))
    {
        return;
    }

    const glm::vec3 hitPoint3D = intersection.m_point;
    const glm::vec2 hitPoint(hitPoint3D.x, hitPoint3D.z);

#if ENABLE_DEBUG_MEMORY_FIELD_HITS
    std::cerr << "Hit at point"
              << " (" << hitPoint3D.x
              << ", " << hitPoint3D.y
              << ", " << hitPoint3D.z
              << ")" << std::endl;
    m_hits.push_back(hitPoint3D);
#endif

    for (CMemoryTile &tile : m_tiles)
    {
        if (tile.MaybeActivate(hitPoint))
        {
#if ENABLE_DEBUG_MEMORY_FIELD_HITS
            std::cerr << "Tile activated!" << std::endl;
#endif
            break;
        }
    }
}

unsigned CMemoryField::GetTileCount() const
{
    return unsigned(m_tiles.size());
}

unsigned CMemoryField::GetTotalScore() const
{
    return m_totalScore;
}

void CMemoryField::GenerateTiles()
{
    std::vector<TileImage> images;
    CRandomTileImageGenerator generator;
    generator.GenerateRandomImagesRange(images, FIELD_WIDTH * FIELD_HEIGHT);

    const CFloatRect backTexRect = m_atlas.GetFrameRect("tile-back.png");

    const float step = TILE_SIZE + TILE_MARGIN;
    const float leftmostX = float(-0.5f * step * FIELD_WIDTH);
    const float topmostY = float(-0.5f * step * FIELD_HEIGHT);

    for (unsigned row = 0; row < FIELD_HEIGHT; ++row)
    {        const float top = float(topmostY + row * step);
        for (unsigned column = 0; column < FIELD_WIDTH; ++column)
        {
            size_t index = row * FIELD_WIDTH + column;
            const float left = float(leftmostX + column * step);
            m_tiles.emplace_back(images.at(index),
                                 glm::vec2{left, top},
                                 glm::vec2{TILE_SIZE, TILE_SIZE});

            CMemoryTile &tile = m_tiles.back();
            const CFloatRect frontTexRect = GetImageFrameRect(tile.GetTileImage());
            tile.SetFrontTextureRect(frontTexRect);
            tile.SetBackTextureRect(backTexRect);
        }
    }
}

void CMemoryField::CheckTilesPair(std::pair<size_t, size_t> indicies)
{
    const int KILL_SCORE_BONUS = 50;
    const int DEACTIVATE_SCORE_FANE = 10;
    CMemoryTile &first = m_tiles[indicies.first];
    CMemoryTile &second = m_tiles[indicies.second];

    if (first.GetTileImage() == second.GetTileImage())
    {
        first.Kill();
        second.Kill();
        m_totalScore += KILL_SCORE_BONUS;
    }
    else
    {
        first.Deactivate();
        second.Deactivate();
        if (m_totalScore <= DEACTIVATE_SCORE_FANE)
        {
            m_totalScore = 0;
        }
        else
        {
            m_totalScore -= DEACTIVATE_SCORE_FANE;
        }
    }
}

CFloatRect CMemoryField::GetImageFrameRect(TileImage image) const
{
    switch (image)
    {
    case TileImage::EXIT_SIGN:
        return m_atlas.GetFrameRect("signExit.png");
    case TileImage::FISH:
        return m_atlas.GetFrameRect("fishSwim2.png");
    case TileImage::FLY:
        return m_atlas.GetFrameRect("flyFly2.png");
    case TileImage::HEART:
        return m_atlas.GetFrameRect("hud_heartFull.png");
    case TileImage::KEY:
        return m_atlas.GetFrameRect("hud_keyYellow.png");
    case TileImage::MUSHROOM:
        return m_atlas.GetFrameRect("mushroomRed.png");
    case TileImage::SNAIL:
        return m_atlas.GetFrameRect("snailWalk1.png");
    case TileImage::SPRINGBOARD:
        return m_atlas.GetFrameRect("springboardUp.png");
    default:
        throw std::runtime_error("Unexpected tile image");
    }
}
