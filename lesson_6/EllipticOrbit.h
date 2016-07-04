#pragma once

#include <tuple>
#include <limits>
#include <boost/math/tools/roots.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

class CEllipticOrbit
{
public:
    CEllipticOrbit(
        double const& largeAxis = 1,        // большая полуось эллипса
        double const& eccentricity = 0,     // эксцентриситет орбиты
        double const& meanMotion = M_PI * 2,// среднее движение (градуcов за единицу времени)
        double const& inclination = 0,      // наклонение орбиты
        double const& periapsisArgument = 0,// параметр перигелия
        double const& longitude = 0,        // долгота восходящего узла
        double const& periapsisEpoch = 0    // начальная эпоха прохождения через перигелий
        ):
    m_largeAxis(largeAxis),
    m_eccentricity(eccentricity),
    m_inclination(inclination),
    m_periapsisArgument(periapsisArgument),
    m_longitude(longitude),
    m_meanMotion(meanMotion),
    m_periapsisEpoch(periapsisEpoch)
    {
    }

    double Eccentricity()const;
    double LargeAxis()const;
    double MeanMotion()const;

    // Средняя аномалия (M)
    double MeanAnomaly(const double &time)const;
    double EccentricityAnomaly(const double &time)const;
    double TrueAnomaly(const double &eccentricityAnomaly)const;
    double RadiusVectorLength(const double &eccentricityAnomaly)const;

    glm::mat4 OrbitRotationMatrix()const;
    glm::vec2 PlanetPosition2D(double const& time)const;

private:
    double m_largeAxis;
    double m_eccentricity;
    double m_inclination;
    double m_periapsisArgument;
    double m_longitude;
    double m_meanMotion;
    double m_periapsisEpoch;
};
