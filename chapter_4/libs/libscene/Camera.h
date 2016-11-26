#pragma once

#include "../libsystem/IInputListener.h"
#include "../libgeometry/Transform.h"
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <set>

class CCamera
        : public IInputListener
        , private boost::noncopyable
{
public:
    explicit CCamera(const glm::vec3 &eye = glm::vec3(0, 0, 1),
                     const glm::vec3 &at = glm::vec3(0, 0, 0),
                     const glm::vec3 &up = glm::vec3(0, 1, 0));

    void SetActive(bool active);
    void Update(float deltaSec);
    bool OnKeyDown(const SDL_KeyboardEvent &event);
    bool OnKeyUp(const SDL_KeyboardEvent &event);
    bool OnMouseMotion(const SDL_MouseMotionEvent &event);

    glm::mat4 GetViewMat4() const;

    float GetMoveSpeed() const;
    void SetMoveSpeed(float value);

private:
    bool m_isActive = true;
    float m_moveSpeed = 1.f;
    glm::vec3 m_position;

    glm::vec3 m_forward;
    glm::vec3 m_up;

    std::set<unsigned> m_keysPressed;
};
