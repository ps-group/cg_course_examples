#include "libchapter4_private.h"
#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>

using glm::vec2;
using glm::vec3;

namespace
{
// Число радианов, на которые вращается камера
//  при перемещении мыши на один пиксель.
const float RADIANS_IN_PIXEL = 1.f / 200.f;

bool ShouldTrackKeyPressed(const SDL_Keysym &key)
{
    switch (key.sym)
    {
    case SDLK_LEFT:
    case SDLK_RIGHT:
    case SDLK_UP:
    case SDLK_DOWN:
    case SDLK_w:
    case SDLK_a:
    case SDLK_s:
    case SDLK_d:
        return true;
    }
    return false;
}

// Возвращает нормализованное направление движения.
vec3 GetMoveDirection(const std::set<unsigned> & keysPressed)
{
    vec3 direction;
    if (keysPressed.count(SDLK_RIGHT) || keysPressed.count(SDLK_d))
    {
        direction.x = -1.f;
    }
    else if (keysPressed.count(SDLK_LEFT) || keysPressed.count(SDLK_a))
    {
        direction.x = +1.f;
    }
    if (keysPressed.count(SDLK_UP) || keysPressed.count(SDLK_w))
    {
        direction.z = +1.f;
    }
    else if (keysPressed.count(SDLK_DOWN) || keysPressed.count(SDLK_s))
    {
        direction.z = -1.f;
    }

    // Если направление ненулевое, нормализуем его.
    if (glm::length(direction) > std::numeric_limits<float>::epsilon())
    {
        direction = glm::normalize(direction);
    }

    return direction;
}
}

CCamera::CCamera(const glm::vec3 &position, const glm::quat &orientation)
{
    m_transform.m_position = position;
    m_transform.m_orientation = orientation;
}

void CCamera::Update(float deltaSec)
{
    const vec3 motion = m_moveSpeed * deltaSec * GetMoveDirection(m_keysPressed);
    m_transform.m_position += motion * m_transform.m_orientation;
}

bool CCamera::OnKeyDown(const SDL_KeyboardEvent &event)
{
    if (ShouldTrackKeyPressed(event.keysym))
    {
        m_keysPressed.insert(unsigned(event.keysym.sym));
        return true;
    }
    return false;
}

bool CCamera::OnKeyUp(const SDL_KeyboardEvent &event)
{
    if (ShouldTrackKeyPressed(event.keysym))
    {
        m_keysPressed.erase(unsigned(event.keysym.sym));
        return true;
    }
    return false;
}

bool CCamera::OnMousePress(const glm::vec2 &pos)
{
    m_prevDragPos = pos;
    m_isDragging = true;
    return true;
}

bool CCamera::OnMouseMotion(const glm::vec2 &pos)
{
    if (!m_isDragging)
    {
        return true;
    }
    const float epsilon = std::numeric_limits<float>::epsilon();
    const vec2 delta = m_prevDragPos - pos;
    m_prevDragPos = pos;

    // Сначала вращаем вверх/вниз вокруг ориентированной оси Ox.
//    if (abs(delta.y) > epsilon)
//    {
//        const vec3 axisX = vec3(1, 0, 0) * m_transform.m_orientation;
//        const float angleUp = delta.y * RADIANS_IN_PIXEL;
//        m_transform.m_orientation = glm::rotate(m_transform.m_orientation, angleUp, axisX);
//    }

    if (abs(delta.x) > epsilon)
    {
        // Затем вращаем влево/вправо вокруг оси направления вверх.
        const vec3 axisUp = glm::axis(m_transform.m_orientation);
//        const vec3 axisY = vec3(0, 1, 0) * m_transform.m_orientation;
        const float angleRight = delta.x * RADIANS_IN_PIXEL
                + glm::angle(m_transform.m_orientation);
        m_transform.m_orientation = glm::angleAxis(angleRight, axisUp);
    }

    return true;
}

bool CCamera::OnMouseUp(const glm::vec2 &)
{
    m_isDragging = false;
    return true;
}

const CTransform3D &CCamera::GetTransform() const
{
    return m_transform;
}

float CCamera::GetMoveSpeed() const
{
    return m_moveSpeed;
}

void CCamera::SetMoveSpeed(float value)
{
    m_moveSpeed = value;
}
