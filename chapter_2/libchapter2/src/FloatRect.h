#pragma once

#include <glm/vec2.hpp>

class CFloatRect
{
public:
    CFloatRect(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);

    glm::vec2 GetTopLeft()const;
    glm::vec2 GetTopRight()const;
    glm::vec2 GetBottomLeft()const;
    glm::vec2 GetBottomRight()const;

private:
    glm::vec2 m_topLeft;
    glm::vec2 m_bottomRight;
};
