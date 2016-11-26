#pragma once

#include "includes/glm-common.hpp"

template <class T>
class CValueRange
{
public:
    T m_min;
    T m_max;

    CValueRange()
        : m_min(T())
        , m_max(T())
    {
    }

    CValueRange(const T &minValue, const T &maxValue)
        : m_min(minValue)
        , m_max(maxValue)
    {
        Normalize();
    }

    // Получает увеличенный диапазон - пересечение двух диапазонов.
    CValueRange GetUnion(const CValueRange &other)const
    {
        CValueRange result;
        result.m_min = (glm::min)(m_min, other.m_min);
        result.m_max = (glm::max)(m_max, other.m_max);

        return result;
    }

    // Получает уменьшенный диапазон - пересечение двух диапазонов.
    CValueRange GetIntersection(const CValueRange &other)const
    {
        CValueRange result;
        result.m_min = (glm::max)(m_min, other.m_min);
        result.m_max = (glm::min)(m_max, other.m_max);
        result.Normalize();

        return result;
    }

    // Если min > max, выполняет присваивание min = max
    void Normalize()
    {
        m_min = (glm::min)(m_min, m_max);
    }
};
