#pragma once

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>
#include "IEntity.h"

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

class CIdentityCube final : public IEntity
{
public:
    CIdentityCube();
    void Update(float deltaTime) final;
    void Draw()const final;

    void SetFaceColor(CubeFace face, const glm::vec3 &color);
    void SetAlpha(float alpha);

private:
    void OutputVertexes()const;

    static const size_t COLORS_COUNT = static_cast<size_t>(CubeFace::NumFaces);
    glm::vec3 m_colors[COLORS_COUNT];
    float m_alpha;
};
