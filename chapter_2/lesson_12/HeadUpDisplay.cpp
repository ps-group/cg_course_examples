#include "stdafx.h"
#include "HeadUpDisplay.h"
#include <iomanip>


using namespace glm;


namespace
{
const char FONT_RESOURCE_PATH[] = "res/memory-trainer/Ubuntu-R.ttf";
const int FONT_POINTS_SIZE = 24;
const vec3 WHITE_RGB = {1, 1, 1};
const vec3 BLACK_RGB = {0, 0, 0};
const float LABEL_MARGIN_X = 15;
const float LABEL_MARGIN_Y = 2;

// Сравнивать действительные числа, обозначающие
//  координаты на сетке пикселей, можно приближённо.
bool ArePixelCoordsCloseEqual(const vec2 &a, const vec2 &b)
{
    const float epsilon = 0.001f;
    return (fabsf(a.x - b.x) < epsilon)
            && (fabsf(a.y - b.y) < epsilon);
}

template <class T>
void DrawWithOrthoView(const ivec2 winSize, T && callback)
{
    // Матрица ортографического проецирования изображения в трёхмерном пространстве
    // из параллелипипеда с размером, равным (size.X x size.Y x 2),
    // на плоскость viewport.
    const mat4 projection = glm::ortho<float>(0, float(winSize.x), float(winSize.y), 0);
    const mat4 identity;

    // Сохраняем и замещаем матрицу проецирования.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(projection));
    glMatrixMode(GL_MODELVIEW);

    // Сохраняем и замещаем матрицу моделирования-вида.
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(identity));

    // Вызываем переданный функтор
    callback();

    // Возвращаем исходное состояние матриц
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/// Привязывает вершины к состоянию OpenGL,
/// затем вызывает 'callback'.
template <class T>
void DoWithBindedArrays(const std::vector<SVertexP2T2> &vertices, T && callback)
{
    // Включаем режимы привязки нужных данных.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Выполняем привязку vertex array, normal array, texture array.
    const size_t stride = sizeof(SVertexP2T2);
    glVertexPointer(2, GL_FLOAT, stride, glm::value_ptr(vertices[0].position));
    glTexCoordPointer(2, GL_FLOAT, stride, glm::value_ptr(vertices[0].texCoord));

    // Выполняем внешнюю функцию.
    callback();

    // Выключаем режимы привязки данных.
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}
}

void CSprite::Update(float)
{
    if (!m_didTesselate)
    {
        Tesselate();
        m_didTesselate = true;
    }
}

void CSprite::Draw() const
{
    if (!m_pTexture)
    {
        return;
    }
    bool hasAlpha = m_pTexture->HasAlpha();
    m_pTexture->DoWhileBinded([&] {
        if (hasAlpha)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        DoWithBindedArrays(m_vertices, [&] {
            glDrawElements(GL_TRIANGLES, GLsizei(m_indicies.size()),
                           GL_UNSIGNED_BYTE, m_indicies.data());
        });
        if (hasAlpha)
        {
            glDisable(GL_BLEND);
        }
    });
}

const CTexture2D &CSprite::GetTexture() const
{
    return *m_pTexture;
}

bool CSprite::HasTexture() const
{
    return bool(m_pTexture);
}

void CSprite::SetTexture(CTexture2DUniquePtr &&pTexture)
{
    m_pTexture = std::move(pTexture);
}

vec2 CSprite::GetPosition() const
{
    return m_position;
}

void CSprite::SetPosition(const vec2 &position)
{
    if (!ArePixelCoordsCloseEqual(m_position, position))
    {
        m_position = position;
        m_didTesselate = false;
    }
}

vec2 CSprite::GetSize() const
{
    return m_size;
}

void CSprite::SetSize(const vec2 &size)
{
    if (!ArePixelCoordsCloseEqual(m_size, size))
    {
        m_size = size;
        m_didTesselate = false;
    }
}

void CSprite::SetAnchorPoint(const vec2 &point)
{
    if (!ArePixelCoordsCloseEqual(m_anchorPoint, point))
    {
        m_anchorPoint = glm::clamp(point, vec2(0.f), vec2(1.f));
        m_didTesselate = false;
    }
}

void CSprite::Tesselate()
{
    const float left = m_position.x - m_anchorPoint.x * m_size.x;
    const float top = m_position.y - m_anchorPoint.y * m_size.y;
    const float right = m_position.x + (1.f - m_anchorPoint.x) * m_size.x;
    const float bottom = m_position.y + (1.f - m_anchorPoint.y) * m_size.y;

    SVertexP2T2 vLeftTop{ vec2(left, top), vec2(0, 0) };
    SVertexP2T2 vRightTop{ vec2(right, top), vec2{1, 0} };
    SVertexP2T2 vLeftBottom{ vec2(left, bottom), vec2{0, 1} };
    SVertexP2T2 vRightBottom{ vec2(right, bottom), vec2{1, 1} };

    m_vertices = { vLeftTop, vRightTop, vLeftBottom, vRightBottom };
    m_indicies = { 0, 1, 2, 1, 3, 2 };
}

CHeadUpDisplay::CHeadUpDisplay(const CHeadUpDisplay::GetWindowSizeFn &getWindowSize)
    : m_getWindowSize(getWindowSize)
{
    m_pFont = CFilesystemUtils::LoadFixedSizeFont(FONT_RESOURCE_PATH, FONT_POINTS_SIZE);
}

void CHeadUpDisplay::SetScore(int value)
{
    if (m_score.value != value)
    {
        m_score.value = value;
        m_score.didRasterize = false;
    }
}

void CHeadUpDisplay::SetTilesLeft(int value)
{
    if (m_tilesLeft.value != value)
    {
        m_tilesLeft.value = value;
        m_tilesLeft.didRasterize = false;
    }
}

void CHeadUpDisplay::Update(float dt)
{
    UpdateIndicator(m_score, "score: ");
    UpdateIndicator(m_tilesLeft, "tiles left: ");

    m_score.sprite.SetPosition(vec2(LABEL_MARGIN_X, LABEL_MARGIN_Y));
    m_score.sprite.Update(dt);

    const float windowWidth = float(m_getWindowSize().x);
    m_tilesLeft.sprite.SetAnchorPoint(vec2(1, 0));
    m_tilesLeft.sprite.SetPosition(vec2(windowWidth - LABEL_MARGIN_X, LABEL_MARGIN_Y));
    m_tilesLeft.sprite.Update(dt);
}

void CHeadUpDisplay::Draw() const
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    DrawWithOrthoView(m_getWindowSize(), [this] {
        m_score.sprite.Draw();
        m_tilesLeft.sprite.Draw();
    });
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

CTexture2DUniquePtr CHeadUpDisplay::RasterizeText(const std::string &text)const
{
    SDLSurfacePtr pSurface = CUtils::RenderUtf8Text(*m_pFont, text, WHITE_RGB);
    const ivec2 surfaceSize = { pSurface->w, pSurface->h };

    const GLenum pixelFormat = GL_RGBA;
    const uint32_t requiredFormat = SDL_PIXELFORMAT_ABGR8888;
    if (pSurface->format->format != requiredFormat)
    {
        pSurface.reset(SDL_ConvertSurfaceFormat(pSurface.get(), requiredFormat, 0));
    }

    auto pTexture = std::make_unique<CTexture2D>(surfaceSize, true);
    pTexture->DoWhileBinded([&] {
        glTexImage2D(GL_TEXTURE_2D, 0, GLint(pixelFormat), pSurface->w, pSurface->h,
            0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    });

    return pTexture;
}

void CHeadUpDisplay::UpdateIndicator(CHeadUpDisplay::SIntegerIndicator &indicator,
                                     const char *textPrefix) const
{
    if (!indicator.didRasterize)
    {
        const std::string text = textPrefix + std::to_string(indicator.value);
        auto pTexture = RasterizeText(text);
        indicator.sprite.SetSize(vec2(pTexture->GetSize()));
        indicator.sprite.SetTexture(std::move(pTexture));
        indicator.didRasterize = true;
    }
}
