#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "libgeometry/FloatRect.h"
#include "Geometry.h"

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

class CStaticGeometry
{
public:
    // Размещение данных сетки примитивов в памяти модели.
    SGeometryLayout m_layout;
    // Буферы с геометрическими данными.
    CGeometrySharedPtr m_pGeometry;
};

/// @class CTesselator служит для разбиения предопределённых поверхностей
///        на треугольники с заданной степенью точности.
class CTesselator
{
public:
    CTesselator() = delete;

    static const unsigned MIN_PRECISION = 4;

    static CStaticGeometry TesselateSphere(unsigned precision);
    static CStaticGeometry TesselateSkybox(const std::vector<CFloatRect> &textureRects);
};
