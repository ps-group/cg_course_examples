#pragma once

#include "Flower.h"
#include <glm/vec2.hpp>

class CMovableFlower : public CFlower
{
public:
    CMovableFlower() = default;

    void Draw()const override;
    bool HitTest(const glm::vec2 &point)const override;

    const glm::vec2 &GetPosition()const;
    void SetPosition(const glm::vec2 &position);

private:
    glm::vec2 m_position;
};
