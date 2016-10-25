#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <cmath>

class CEllipticOrbit
{
public:
    CEllipticOrbit(
        double const& largeAxis,     // большая полуось эллипса
        double const& eccentricity,  // эксцентриситет орбиты
        double const& meanMotion,    // среднее движение (градуcов за единицу времени)
        double const& periapsisEpoch // начальная эпоха прохождения через перигелий
        ):
    m_largeAxis(largeAxis),
    m_eccentricity(eccentricity),
    m_meanMotion(meanMotion),
    m_periapsisEpoch(periapsisEpoch)
    {
    }

    double Eccentricity()const;
    double LargeAxis()const;

    glm::vec2 PlanetPosition2D(double const& time)const;

private:
    double MeanAnomaly(const double &time)const;
    double EccentricityAnomaly(const double &time)const;
    double TrueAnomaly(const double &eccentricityAnomaly)const;
    double RadiusVectorLength(const double &eccentricityAnomaly)const;

    double m_largeAxis;
    double m_eccentricity;
    double m_meanMotion;
    double m_periapsisEpoch;
};
