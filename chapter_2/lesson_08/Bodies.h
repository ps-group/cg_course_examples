#pragma once

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

enum class CubeFace
{
    Front = 0,
    Back,
    Top,
    Bottom,
    Left,
    Right,

    NumFaces
};

class CIdentityCube
{
public:
    CIdentityCube();
    void Update(float deltaTime);
    void Draw()const;

    void SetFaceColor(CubeFace face, const glm::vec3 &color);

private:
    static const size_t COLORS_COUNT = static_cast<size_t>(CubeFace::NumFaces);
    glm::vec3 m_colors[COLORS_COUNT];
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
