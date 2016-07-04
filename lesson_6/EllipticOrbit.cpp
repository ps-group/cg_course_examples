#include "stdafx.h"
#include "EllipticOrbit.h"

namespace
{
// Матчасть взята отсюда: http://www.astronet.ru/db/msg/1190817/node21.html#ll60

// Уравнение кеплера, задающее связь между эксцентрической аномалией,
// эксцентриситетом орбиты и средней аномалией
//      M = E - e * sin(E)
// где M - средняя аномалия, e - эксцентриситет орбиты, E - эксцентрическая аномалия
// уравнение `M = E - e * sin(E)` преобразуется так, чтобы слева было число 0,
// а справа - некоторая функция F(E):
//      0 = E - M - e * sin(E)
// Функтор возвращает функцию от E, а также две её производные функции.
//      F(E) = E - M - e * sin(E)
//      F'(E) = 1 - e * cos(E)
//      F''(E) = e * sin(E)
using FunctionSnapshot = boost::math::tuple<double, double, double>;
using EquationFunction = std::function<FunctionSnapshot(const double &x)>;

EquationFunction MakeKeplerEquationFunction(double meanAnomaly, double eccentricity)
{
    return [=](const double &x) {
        return boost::math::make_tuple(
                    x - meanAnomaly - eccentricity * sin(x),    // функция
                    1 - eccentricity * cos(x),                  // её первая производная
                    eccentricity * sin(x));                     // её вторая производная
    };
}

// Функция выполняет численое решение уравнения кеплера,
// вычисляя эксцентрическую аномалию
// при известных средней аномалии и эксцентриситете орбиты
// В качестве решения используется метод Halley
// (http://en.wikipedia.org/wiki/Halley's_method),
// используя соответствующие алгоритмы библиотеки boost.
double SolveKeplerEquation(double const& meanAnomaly, double const& eccentricity)
{
    const int digits = (std::numeric_limits<double>::digits) >> 1;
    // ограничиваем максимальное число итераций,
    // поскольку мы рисуем систему планет в реальном времени.
    boost::uintmax_t maxIteractions = 1000;

    return boost::math::tools::halley_iterate(
        MakeKeplerEquationFunction(meanAnomaly, eccentricity),
        meanAnomaly,                // первое приближение корня
        meanAnomaly - eccentricity, // минимальное значение корня
        meanAnomaly + eccentricity, // максимальное значение корня
        digits,                        // число разрядов
        maxIteractions);            // наибольшее число итераций
}
}

double CEllipticOrbit::Eccentricity() const
{
    return m_eccentricity;
}

double CEllipticOrbit::LargeAxis() const
{
    return m_largeAxis;
}

double CEllipticOrbit::MeanMotion() const
{
    return m_meanMotion;
}

double CEllipticOrbit::MeanAnomaly(const double &time) const
{
    const double anomaly = 2 * M_PI * m_meanMotion * (time - m_periapsisEpoch);
    const double wrappedAnomaly = fmod(anomaly, M_PI + M_PI);
    return wrappedAnomaly;
}

double CEllipticOrbit::EccentricityAnomaly(const double &time) const
{
    return SolveKeplerEquation(MeanAnomaly(time), m_eccentricity);
}

double CEllipticOrbit::TrueAnomaly(const double &eccentricityAnomaly) const
{
    // Тангенс половинчатого угла
    const double tg_v_2 = sqrt((1 + m_eccentricity) / (1 - m_eccentricity)) * tan(eccentricityAnomaly / 2);
    // Половинчатый угол
    const double v_2 = atan(tg_v_2);
    // Истинная аномалия
    return v_2 + v_2;
}

double CEllipticOrbit::RadiusVectorLength(const double &eccentricityAnomaly) const
{
    return m_largeAxis * (1 - m_eccentricity * cos(eccentricityAnomaly));
}

glm::mat4 CEllipticOrbit::OrbitRotationMatrix() const
{
    const float cl = cosf(float(m_longitude));
    const float sl = sinf(float(m_longitude));

    const float cw = cosf(float(m_periapsisArgument));
    const float sw = sinf(float(m_periapsisArgument));

    const float ci = cosf(float(m_inclination));
    const float si = sinf(float(m_inclination));

    return {
        // Столбец 1
        cl * cw - sl * sw * ci,
        sl * cw + cl * sw * ci,
        sw * si,
        0.f,
        // Столбец 2
        -cl * sw - sl * cw * ci,
        -sl * cw + cl * cw * ci,
        cw * si,
        0.f,
        // Столбец 3
        sl * si,
        cl * si,
        ci,
        0.f,
        // Столбец 4
        0.f, 0.f, 0.f, 1.f
    };
}

glm::vec2 CEllipticOrbit::PlanetPosition2D(const double &time) const
{
    const double e = EccentricityAnomaly(time);
    const float r = float(RadiusVectorLength(e));
    const float v = float(TrueAnomaly(e));
    return { r * cosf(v), r * sinf(v) };
}
