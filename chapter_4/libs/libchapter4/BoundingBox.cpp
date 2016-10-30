#include "libchapter4_private.h"
#include "BoundingBox.h"

CBoundingBox::CBoundingBox(const glm::vec3 &lowerBounds, const glm::vec3 &upperBounds)
    : m_lowerBounds(lowerBounds)
    , m_upperBounds(upperBounds)
{
}

void CBoundingBox::Unite(const CBoundingBox &other)
{
    Unite(other.m_lowerBounds, other.m_upperBounds);
}

void CBoundingBox::Unite(const glm::vec3 &lowerBounds, const glm::vec3 &upperBounds)
{
    m_lowerBounds = glm::min(m_lowerBounds, lowerBounds);
    m_upperBounds = glm::max(m_upperBounds, upperBounds);
}
