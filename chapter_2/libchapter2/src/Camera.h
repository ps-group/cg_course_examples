#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <SDL2/SDL_events.h>
#include <boost/noncopyable.hpp>
#include <set>

class CCamera : private boost::noncopyable
{
public:
    explicit CCamera(float rotationRadians, float distance);

    void Update(float deltaSec);
    bool OnKeyDown(const SDL_KeyboardEvent &event);
    bool OnKeyUp(const SDL_KeyboardEvent &event);

    void SetDirection(const glm::vec3 &direction);

    glm::mat4 GetViewTransform() const;

private:
    glm::vec3 m_direction;
    float m_rotationRadians = 0;
    float m_distance = 1;
    std::set<unsigned> m_keysPressed;
};
