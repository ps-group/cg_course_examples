#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/*
Класс "Луч", характеризующийся точкой испускания и направлением.
  В момент времени t=0 луч находится в точке start.
  За промежуток времени t=1 луч проходит расстояние direction.
*/
class CRay
{
public:
    CRay() = default;
    explicit CRay(const glm::vec3 &start, const glm::vec3 &direction);

    glm::vec3 GetPointAtTime(float time)const;

    const glm::vec3 &GetStart()const;
    const glm::vec3 &GetDirection()const;

    CRay GetTransformedCopy(const glm::mat4 &transform)const;

private:
    glm::vec3 m_start;
    glm::vec3 m_direction;
};

struct SRayIntersection
{
    // Время пересечения с лучём (по временной шкале луча).
    float m_time;
    // Точка пересечения
    glm::vec3 m_point;
};

/*
Геометрический объект "бесконечная плоскость",
  который задаётся уравнением плоскости из 4-х коэффициентов.
*/
class CPlane
{
public:
    // Три точки определяют плоскость, из них могут быть восстановлены
    //  коэффициенты уравнения плоскости.
    explicit CPlane(const glm::vec3 &point0,
                    const glm::vec3 &point1,
                    const glm::vec3 &point2);

    explicit CPlane(const glm::vec4 &planeEquation);

    bool Hit(CRay const& ray, SRayIntersection &intersection)const;

private:
    // Четырехмерный вектор, хранящий коэффициенты уравнения плоскости
    glm::vec4 m_planeEquation;
};
