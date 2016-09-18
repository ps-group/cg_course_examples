#pragma once

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>
#include <boost/noncopyable.hpp>

class CIdentityCube
{
public:
    void Update(float deltaTime);
    void Draw()const;
};

class CAnimatedCube : public CIdentityCube
{
public:
    void Update(float deltaTime);
    void Draw()const;

private:
    enum Animation
    {
        Rotating,
        Pulse,
        Bounce,
    };

    glm::mat4 GetAnimationTransform()const;

    static const float ANIMATION_STEP_SECONDS;
    Animation m_animation = Rotating;
    float m_animationPhase = 0;
};
