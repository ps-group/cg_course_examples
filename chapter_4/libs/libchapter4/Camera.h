#pragma once

#include "IInputListener.h"
#include "Transform.h"
#include <boost/noncopyable.hpp>
#include <set>

class CCamera
        : public IInputListener
        , private boost::noncopyable
{
public:
    explicit CCamera(const glm::vec3 &position = glm::vec3(),
                     const glm::quat &orientation = glm::quat());

    void Update(float deltaSec);
    bool OnKeyDown(const SDL_KeyboardEvent &event);
    bool OnKeyUp(const SDL_KeyboardEvent &event);
    bool OnMousePress(const glm::vec2 &pos);
    bool OnMouseMotion(const glm::vec2 &pos);
    bool OnMouseUp(const glm::vec2 &pos);

    const CTransform3D &GetTransform() const;

    float GetMoveSpeed() const;
    void SetMoveSpeed(float value);

private:
    float m_moveSpeed = 1.f;
    CTransform3D m_transform;
    bool m_isDragging = false;
    glm::vec2 m_prevDragPos;
    std::set<unsigned> m_keysPressed;
};
