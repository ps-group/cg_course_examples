#pragma once

#include "IInputListener.h"
#include "Transform.h"
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <set>

class CCamera
        : public IInputListener
        , private boost::noncopyable
{
public:
    explicit CCamera(const glm::vec3 &position = glm::vec3(),
                     const glm::vec3 &up = glm::vec3(0, 1, 0),
                     const glm::vec3 &forward = glm::vec3(0, 0, 1));

    void Update(float deltaSec);
    bool OnKeyDown(const SDL_KeyboardEvent &event);
    bool OnKeyUp(const SDL_KeyboardEvent &event);
    bool OnMouseMotion(const SDL_MouseMotionEvent &event);

    glm::mat4 GetViewMat4() const;

    float GetMoveSpeed() const;
    void SetMoveSpeed(float value);

private:
    float m_moveSpeed = 1.f;
    glm::vec3 m_position;

    glm::vec3 m_up;
    glm::vec3 m_forward;

    std::set<unsigned> m_keysPressed;
};
