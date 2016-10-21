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
    explicit CCamera(const glm::vec3 &position = glm::vec3());

    void Update(float deltaSec);
    bool OnKeyDown(const SDL_KeyboardEvent &event);
    bool OnKeyUp(const SDL_KeyboardEvent &event);
    bool OnMousePress(const glm::vec2 &pos);
    bool OnMouseMotion(const glm::vec2 &pos);
    bool OnMouseUp(const glm::vec2 &pos);

    glm::mat4 GetViewMat4() const;

    float GetMoveSpeed() const;
    void SetMoveSpeed(float value);

private:
    float m_moveSpeed = 1.f;
    glm::vec3 m_position;
    float m_yaw = 0.f;
    float m_pitch = 0.f;
    float m_roll = 0.f;

    boost::optional<glm::vec2> m_prevMousePos;
    std::set<unsigned> m_keysPressed;
};
