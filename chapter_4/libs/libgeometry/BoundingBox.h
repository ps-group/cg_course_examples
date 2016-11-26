#pragma once

#include "ValueRange.h"

// Представляет ограничивающий параллелипипед, выравненный по осям координат
//  англ. Axis Aligned Bounding Box, или просто AABB.
class CBoundingBox
{
public:
    CBoundingBox() = default;
    explicit CBoundingBox(const glm::vec3 &lowerBounds, const glm::vec3 &upperBounds);

    // Объединяет параллелепипед с другим параллелепипедом.
    void Unite(const CBoundingBox &other);
    void Unite(const glm::vec3 &lowerBounds, const glm::vec3 &upperBounds);

private:
    CValueRange<glm::vec3> m_bounds;
};
