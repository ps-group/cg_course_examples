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

// В OpenGL используется левосторонняя система координат,
//  следовательно, вектор `right` должен быть таким, чтобы с его конца
//  кратчайший поворот от вектора `m_up` к вектору `m_forward` был виден
//  по часовой стрелке, то есть совпадает с векторным произведением.
// См. http://www.gamedev.ru/code/forum/?id=43526
vec3 GetRightDirection(const vec3 &up, const vec3 &forward)
{
    return glm::cross(up, forward);
}

// По причинам, описанным выше, добавляем
//  знак минус к векторному произведению.
vec3 GetForwardDirection(const vec3 &up, const vec3 &right)
{
    return -glm::cross(up, right);
}

// По причинам, описанным выше, мы не инвертируем векторное произведение.
vec3 GetUpDirection(const vec3 &forward, const vec3 &right)
{
    return glm::cross(forward, right);
}
}

CCamera::CCamera(const glm::vec3 &eye, const glm::vec3 &at, const glm::vec3 &up)
    : m_position(eye)
    , m_forward(glm::normalize(at - eye))
{
    // Угол между `forward` и `up` может быть не равен 90 градусам,
    //  поэтому мы дважды вычисляем векторное произведение
    //  для получения истинного направления `up`.
    const vec3 right = glm::normalize(GetRightDirection(up, m_forward));
    m_up = glm::normalize(GetUpDirection(m_forward, right));
}

void CCamera::SetActive(bool active)
{
    m_isActive = active;
}

void CCamera::Update(float deltaSec)
{
    const vec3 localDir = GetMoveDirection(m_keysPressed);
    const vec3 right = GetRightDirection(m_up, m_forward);
    const vec3 orientedDir = right * localDir.x
            + m_up * localDir.y + m_forward * localDir.z;

    m_position += m_moveSpeed * deltaSec * orientedDir;
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

bool CCamera::OnMouseMotion(const SDL_MouseMotionEvent &event)
{
    if (m_isActive)
    {
        const vec2 delta = vec2(-event.xrel, -event.yrel);

        // Движение мыши по оси X изменяет рысканье (yaw),
        //  то есть поворачивает вектор forward вокруг up.
        const float deltaYaw = delta.x * RADIANS_IN_PIXEL;
        m_forward = glm::normalize(glm::rotate(m_forward, deltaYaw, m_up));

        // Движение мыши по оси Y изменяет курс (roll),
        //  то есть поворачивает вектора up вокруг right
        //  и восстанавливаем вектор forward.
        const float deltaRoll = -delta.y * RADIANS_IN_PIXEL;
        const vec3 right = GetRightDirection(m_up, m_forward);
        m_up = glm::normalize(glm::rotate(m_up, deltaRoll, right));
        m_forward = GetForwardDirection(m_up, right);
    }

    return m_isActive;
}

mat4 CCamera::GetViewMat4() const
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

float CCamera::GetMoveSpeed() const
{
    return m_moveSpeed;
}

void CCamera::SetMoveSpeed(float value)
{
    m_moveSpeed = value;
}
