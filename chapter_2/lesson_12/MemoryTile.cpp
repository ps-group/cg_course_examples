#include "stdafx.h"
#include "MemoryTile.h"

namespace
{
// Время проигрывания анимации, секунд.
const float ANIMATION_TIME = 0.3f;
const float ANIMATION_SPEED = 1.f / ANIMATION_TIME;

/// Привязывает вершины к состоянию OpenGL,
/// затем вызывает 'callback'.
template <class T>
void DoWithBindedArrays(const std::vector<SVertexP3NT2> &vertices, T && callback)
{
    // Включаем режимы привязки нужных данных.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Выполняем привязку vertex array, normal array, texture array.
    const size_t stride = sizeof(SVertexP3NT2);
    glVertexPointer(3, GL_FLOAT, stride, glm::value_ptr(vertices[0].position));
    glNormalPointer(GL_FLOAT, stride, glm::value_ptr(vertices[0].normal));
    glTexCoordPointer(2, GL_FLOAT, stride, glm::value_ptr(vertices[0].texCoord));

    // Выполняем внешнюю функцию.
    callback();

    // Выключаем режимы привязки данных.
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

// Тайлы лежат в плоскости Oxz, нормаль сонаправлена с осью Oy.
SVertexP3NT2 MakeVertex(const glm::vec2 &xz, float normalY)
{
    SVertexP3NT2 vertex;
    vertex.position = { xz.x, 0.f, xz.y };
    vertex.normal = { 0.f, normalY, 0.f };
    return vertex;
}
}

CTwoSideQuad::CTwoSideQuad(const glm::vec2 &leftTop, const glm::vec2 &size)
{
    SVertexP3NT2 vLeftTopFront = MakeVertex(leftTop, 1.f);
    SVertexP3NT2 vRightTopFront = MakeVertex(leftTop + glm::vec2{ size.x, 0.f }, 1.f);
    SVertexP3NT2 vLeftBottomFront = MakeVertex(leftTop + glm::vec2{ 0.f, size.y }, 1.f);
    SVertexP3NT2 vRightBottomFront = MakeVertex(leftTop + glm::vec2{ size.x, size.y }, 1.f);

    SVertexP3NT2 vLeftTopBack = MakeVertex(leftTop, -1.f);
    SVertexP3NT2 vRightTopBack = MakeVertex(leftTop + glm::vec2{ size.x, 0.f }, -1.f);
    SVertexP3NT2 vLeftBottomBack = MakeVertex(leftTop + glm::vec2{ 0.f, size.y }, -1.f);
    SVertexP3NT2 vRightBottomBack = MakeVertex(leftTop + glm::vec2{ size.x, size.y }, -1.f);

    m_vertices = { vLeftTopFront, vRightTopFront, vLeftBottomFront, vRightBottomFront,
                   vLeftTopBack, vRightTopBack, vLeftBottomBack, vRightBottomBack };
    m_indicies = { 0, 1, 2, 1, 3, 2,
                   6, 5, 4, 6, 7, 5 };
}

void CTwoSideQuad::Draw() const
{
    DoWithBindedArrays(m_vertices, [this] {
        glDrawElements(GL_TRIANGLES, GLsizei(m_indicies.size()),
                       GL_UNSIGNED_BYTE, m_indicies.data());
    });
}

void CTwoSideQuad::SetFrontTextureRect(const CFloatRect &rect)
{
    m_vertices[0].texCoord = rect.GetTopLeft();
    m_vertices[1].texCoord = rect.GetTopRight();
    m_vertices[2].texCoord = rect.GetBottomLeft();
    m_vertices[3].texCoord = rect.GetBottomRight();
}

void CTwoSideQuad::SetBackTextureRect(const CFloatRect &rect)
{
    m_vertices[4].texCoord = rect.GetTopLeft();
    m_vertices[5].texCoord = rect.GetTopRight();
    m_vertices[6].texCoord = rect.GetBottomLeft();
    m_vertices[7].texCoord = rect.GetBottomRight();
}

CMemoryTile::CMemoryTile(TileImage tileImage,
                         const glm::vec2 &leftTop, const glm::vec2 &size)
    : CTwoSideQuad(-0.5f * size, size)
    , m_tileImage(tileImage)
    , m_bounds(leftTop, leftTop + size)
    , m_animationCounter(ANIMATION_SPEED)
{
    m_animationCounter.Restart();
}

CAnimationCounter::CAnimationCounter(float changeSpeed)
    : m_phase(1.f)
    , m_changeSpeed(changeSpeed)
{
}

float CAnimationCounter::GetPhase() const
{
    return m_phase;
}

bool CAnimationCounter::IsActive() const
{
    return m_isActive;
}

void CAnimationCounter::Restart()
{
    m_phase = 0;
    m_isActive = true;
}

void CAnimationCounter::Update(float deltaSeconds)
{
    if (!m_isActive)
    {
        return;
    }
    const float maxPhase = 1.f;
    const float delta = m_changeSpeed * deltaSeconds;
    if ((maxPhase - m_phase) < delta)
    {
        m_phase = maxPhase;
        m_isActive = false;
    }
    else
    {
        m_phase += delta;
    }
}

TileImage CMemoryTile::GetTileImage() const
{
    return m_tileImage;
}

void CMemoryTile::SetTileImage(TileImage tileImage)
{
    m_tileImage = tileImage;
}

bool CMemoryTile::IsFrontFaced() const
{
    return (m_state == State::FacedFront) && !m_animationCounter.IsActive();
}

bool CMemoryTile::IsAlive() const
{
    return (m_state != State::Dead) || m_animationCounter.IsActive();
}

bool CMemoryTile::MaybeActivate(const glm::vec2 &point)
{
    if (m_animationCounter.IsActive() || !m_bounds.Contains(point))
    {
        return false;
    }

    if (m_state == State::FacedBack)
    {
        m_state = State::FacedFront;
        m_animationCounter.Restart();
        return true;
    }
    return false;
}

void CMemoryTile::Deactivate()
{
    assert(m_state == State::FacedFront);
    m_state = State::Teasing;
    m_animationCounter.Restart();
}

void CMemoryTile::Kill()
{
    if (m_state != State::Dead && !m_animationCounter.IsActive())
    {
        m_state = State::Dead;
        m_animationCounter.Restart();
    }
}

void CMemoryTile::Update(float dt)
{
    m_animationCounter.Update(dt);

    // После завершения анимации переключаем состояние Teasing
    //  на FacedBack, перезапуская анимацию переворота.
    if (m_state == State::Teasing && !m_animationCounter.IsActive())
    {
        m_state = State::FacedBack;
        m_animationCounter.Restart();
    }
}

void CMemoryTile::Draw() const
{
    const glm::vec2 offset = m_bounds.GetTopLeft() + 0.5f * m_bounds.GetSize();
    const glm::vec3 zAxis = {0, 0, 1};
    const glm::vec3 yAxis = {0, 1, 0};
    glm::mat4 transform;
    transform = glm::translate(transform, {offset.x, 0.f, offset.y});

    const float phase = m_animationCounter.GetPhase();

    switch (m_state)
    {
    case State::FacedBack:
        transform = glm::rotate(transform, (phase + 1.f) * float(M_PI), zAxis);
        break;
    case State::FacedFront:
        transform = glm::rotate(transform, phase * float(M_PI), zAxis);
        break;
    case State::Teasing:
    {
        const float deviation = 0.1f - 0.2f * fabsf(0.5f - phase);
        transform = glm::rotate(transform, float(M_PI), zAxis);
        transform = glm::rotate(transform, deviation * float(M_PI), yAxis);
        break;
    }
    case State::Dead:
        transform = glm::scale(transform, glm::vec3(1.f - phase));
        break;
    }

    glPushMatrix();
    glMultMatrixf(glm::value_ptr(transform));
    CTwoSideQuad::Draw();
    glPopMatrix();
}
