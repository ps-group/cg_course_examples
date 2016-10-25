#pragma once
#include <cmath>

namespace scale
{

// Размер астрономической единицы (а.е., astronomical unit)
static const float AU_SIZE = 2.0f;

// Реальный размер Земли в а.е. равен 0.0000421,
//  но мы масштабируем систему для лучшей наглядности.
static const float EARTH_SIZE_IN_AU = 0.01f;

// Возвращает размер космического тела,
//  - для наглядности масштабы изменены
//  - для уменьшения различий между большими и малыми телами
//    к размеру тела применяется sqrt
inline float GetPlanetSizeInAU(float bodySize)
{
    return EARTH_SIZE_IN_AU * AU_SIZE * std::sqrt(bodySize);
}

}
