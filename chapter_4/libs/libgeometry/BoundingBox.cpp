#include "includes/glm-common.hpp"
#include "BoundingBox.h"

CBoundingBox::CBoundingBox(const glm::vec3 &lowerBounds, const glm::vec3 &upperBounds)
    : m_bounds(lowerBounds, upperBounds)
{
}

void CBoundingBox::Unite(const CBoundingBox &other)
{
    Unite(other.m_bounds.m_min, other.m_bounds.m_max);
}

void CBoundingBox::Unite(const glm::vec3 &lowerBounds, const glm::vec3 &upperBounds)
{
    m_bounds = m_bounds.GetUnion(CValueRange<glm::vec3>(lowerBounds, upperBounds));
}
