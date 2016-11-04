#include "includes/glm-common.hpp"
#include "FloatRect.h"


CFloatRect::CFloatRect(const glm::vec2 &topLeft, const glm::vec2 &bottomRight)
    : m_topLeft(topLeft)
    , m_bottomRight(bottomRight)
{
    assert(m_bottomRight.x >= m_topLeft.x
           && m_bottomRight.y >= m_topLeft.y);
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

glm::vec2 CFloatRect::GetSize() const
{
    return m_bottomRight - m_topLeft;
}

bool CFloatRect::Contains(const glm::vec2 &point) const
{
    return (m_topLeft.x <= point.x && point.x <= m_bottomRight.x
         && m_topLeft.y <= point.y && point.y <= m_bottomRight.y);
}

CFloatRect CFloatRect::GetScaled(const glm::vec2 &factors) const
{
    CFloatRect copy(*this);

    // Домножаем для масштабирования.
    copy.m_topLeft *= factors;
    copy.m_bottomRight *= factors;

    return copy;
}

void CFloatRect::MoveTo(const glm::vec2 &topLeft)
{
    const glm::vec2 size = GetSize();
    m_topLeft = topLeft;
    m_bottomRight = topLeft + size;
}
