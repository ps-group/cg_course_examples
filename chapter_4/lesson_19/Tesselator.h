#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "libchapter4/FloatRect.h"

class CMeshP3NT2;
class CTexture2DAtlas;

enum class CubeFace
{
    Front = 0,
    Back,
    Top,
    Bottom,
    Left,
    Right,

    NumFaces
};

/// @class CTesselator служит для разбиения предопределённых поверхностей
///        на треугольники с заданной степенью точности.
class CTesselator
{
public:
    CTesselator() = delete;

    static const unsigned MIN_PRECISION = 4;

    static std::unique_ptr<CMeshP3NT2> TesselateSphere(unsigned precision);
    static std::unique_ptr<CMeshP3NT2> TesselateSkybox(const std::vector<CFloatRect> &textureRects);
};
