#include "libchapter4_private.h"
#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;
using glm::quat;

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

CCamera::CCamera(const glm::vec3 &position)
    : m_position(position)
{
}

void CCamera::Update(float deltaSec)
{
    const vec3 motion = m_moveSpeed * deltaSec * GetMoveDirection(m_keysPressed);
    m_position += motion;
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

bool CCamera::OnMousePress(const glm::vec2 &)
{
    return true;
}

bool CCamera::OnMouseMotion(const glm::vec2 &pos)
{
    boost::optional<vec2> prevMousePos = m_prevMousePos;
    m_prevMousePos = pos;
    if (!prevMousePos)
    {
        return true;
    }

    const vec2 delta = *prevMousePos - pos;
    // Движение мыши по оси X изменяет рысканье (yaw).
    m_yaw -= delta.x * RADIANS_IN_PIXEL;
    // Движение мыши по оси Y изменяет курс (roll)
    m_roll -= delta.y * RADIANS_IN_PIXEL;

    return true;
}

bool CCamera::OnMouseUp(const glm::vec2 &)
{
    return true;
}

mat4 CCamera::GetViewMat4() const
{
    const mat4 rotationMatrix = glm::yawPitchRoll(m_yaw, m_pitch, m_roll);
    const mat4 translateMatrix = glm::translate(mat4(), m_position);

    return translateMatrix * rotationMatrix;
}

float CCamera::GetMoveSpeed() const
{
    return m_moveSpeed;
}

void CCamera::SetMoveSpeed(float value)
{
    m_moveSpeed = value;
}
