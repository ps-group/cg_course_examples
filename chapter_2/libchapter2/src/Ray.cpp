#include "libchapter2_private.h"
#include "Ray.h"

using namespace glm;

CRay::CRay(const vec3 &start, const vec3 &direction)
    : m_start(start)
    , m_direction(direction)
{
}

vec3 CRay::GetPointAtTime(float time) const
{
    return m_start + m_direction * time;
}

const vec3 &CRay::GetStart() const
{
    return m_start;
}

const vec3 &CRay::GetDirection() const
{
    return m_direction;
}

CRay CRay::GetTransformedCopy(const mat4 &transform) const
{
    const vec4 start = transform * vec4(m_start, 1.f);
    const vec4 direction = transform * vec4(m_direction, 0.f);

    // Проверяем, что флаг "w" у точки всегда остался равным 1.
    //  При умножении на матрицу перспективного преобразования
    //  флаг может измениться.
    assert(fabsf(start.w - 1.0f) <= std::numeric_limits<float>::epsilon());

    return CRay(vec3(start),
                vec3(direction));
}

CPlane::CPlane(const vec3 &point0, const vec3 &point1, const vec3 &point2)
{
    // Вычисляем два ребра треугольника
    const vec3 edge01 = point1 - point0;
    const vec3 edge20 = point0 - point2;

    // Нормаль к плоскости треугольника и уравнение его плоскости
    const vec3 normal = cross(edge20, edge01);
    m_planeEquation = vec4(normal, -dot(normal, point0));
}

CPlane::CPlane(const vec4 &planeEquation)
    : m_planeEquation(planeEquation)
{
}

bool CPlane::Hit(const CRay &ray, SRayIntersection &intersection) const
{
    // Величина, меньше которой модуль скалярного произведения вектора направления луча и
    // нормали плоскости означает параллельность луча и плоскости
    const float EPSILON = std::numeric_limits<float>::epsilon();

    // Нормаль к плоскости в системе координат объекта
    const glm::vec3 normalInObjectSpace(m_planeEquation);

    // Скалярное произведение направления луча и нормали к плоскости
    const float normalDotDirection = glm::dot(ray.GetDirection(), normalInObjectSpace);

    // Если скалярное произведение близко к нулю, луч параллелен плоскости
    if (fabs(normalDotDirection) < EPSILON)
    {
        return false;
    }

    /*
    Находим время пересечения луча с плоскостью, подставляя в уравнение плоскости точку испускания луча
    и деление результата на ранее вычисленное сканярное произведение направления луча и нормали к плоскости
    */
    const float hitTime = -glm::dot(glm::vec4(ray.GetStart(), 1), m_planeEquation) / normalDotDirection;

    // Нас интересует только пересечение луча с плоскостью в положительный момент времени,
    // поэтому находящуюся "позади" точки испускания луча точку пересечения мы за точку пересечения не считаем
    // Сравнение с величиной EPSILON, а не с 0 нужно для того, чтобы позволить
    // лучам, испущенным с плоскости, оторваться от нее.
    // Это необходимо при обработке вторичных лучей для построения теней и отражений и преломлений
    if (hitTime <= EPSILON)
    {
        return false;
    }

    // Вычисляем точку столкновения с лучом в системе координат сцены в момент столкновения
    const glm::vec3 hitPoint = ray.GetPointAtTime(hitTime);
    intersection.m_time = hitTime;
    intersection.m_point = hitPoint;

    return true;
}
