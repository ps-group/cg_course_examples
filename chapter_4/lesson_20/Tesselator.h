#pragma once

#include <functional>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "MeshP3NT2.h"

/// @class CTesselator служит для разбиения предопределённых поверхностей
///        на треугольники с заданной степенью точности.
class CTesselator
{
public:
    CTesselator() = delete;

    static const unsigned MIN_PRECISION = 4;

    static std::unique_ptr<CMeshP3NT2> TesselateSphere(unsigned precision);
};
