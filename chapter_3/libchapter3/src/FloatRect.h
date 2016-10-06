#pragma once

#include <glm/vec2.hpp>

class CFloatRect
{
public:
    CFloatRect() = default;
    CFloatRect(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);

    glm::vec2 GetTopLeft()const;
    glm::vec2 GetTopRight()const;
    glm::vec2 GetBottomLeft()const;
    glm::vec2 GetBottomRight()const;
    glm::vec2 GetSize()const;

    bool Contains(const glm::vec2 &point)const;
    CFloatRect GetScaled(const glm::vec2 &factors)const;

    void MoveTo(const glm::vec2 &topLeft);

private:
    glm::vec2 m_topLeft;
    glm::vec2 m_bottomRight;
};
