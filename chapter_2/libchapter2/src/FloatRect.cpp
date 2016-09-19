#include "libchapter2_private.h"
#include "FloatRect.h"

CFloatRect::CFloatRect(const glm::vec2 &topLeft, const glm::vec2 &bottomRight)
    : m_topLeft(topLeft)
    , m_bottomRight(bottomRight)
{
}

glm::vec2 CFloatRect::GetTopLeft() const
{
    return m_topLeft;
}

glm::vec2 CFloatRect::GetTopRight() const
{
    return { m_bottomRight.x, m_topLeft.y };
}

glm::vec2 CFloatRect::GetBottomLeft() const
{
    return { m_topLeft.x, m_bottomRight.y };
}

glm::vec2 CFloatRect::GetBottomRight() const
{
    return m_bottomRight;
}
