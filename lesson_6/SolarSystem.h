#pragma once

#include "EllipticOrbit.h"
#include <glm/vec3.hpp>

class CSolarSystem
{
public:
    CSolarSystem();

    void Update(float deltaTime);
    void Draw();

    float GetViewScale()const;
    float GetTimeSpeed()const;
    unsigned GetYear()const;
    void ZoomIn();
    void ZoomOut();
    void SpeedupTime();
    void SlowdownTime();

private:
    void DrawSun();
    void DrawOrbit(const CEllipticOrbit &orbit);
    void DrawPlanets();

    float m_time;
    float m_timeSpeed;
    unsigned m_currentYear;
    float m_viewScale;

    struct PlanetInfo
    {
        CEllipticOrbit orbit;
        glm::vec3 color;
        float size;
    };
    std::vector<PlanetInfo> m_planets;
};
