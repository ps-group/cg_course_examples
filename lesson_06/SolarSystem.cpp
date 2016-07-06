#include "stdafx.h"
#include "SolarSystem.h"

namespace
{
const unsigned INITIAL_YEAR = 2012;
const float DEFAULT_TIME_SPEED = 0.1f;
const float MIN_TIME_SPEED = -20;
const float MAX_TIME_SPEED = +20;
const float TIME_ADJUSTMENT = 0.02f;

// 40 пикселей на астрономическую единицу
const float DEFAULT_SCALE = 100.f;
const float MIN_SCALE = 5;
const float MAX_SCALE = 500;
const float SCALE_FACTOR = 1.f;

double GetRadians(double degrees, double seconds = 0)
{
    return glm::radians(degrees + seconds / 60.0);
}

glm::vec3 FromRGB(unsigned colorCode)
{
    unsigned r = (colorCode >> 16) % 256;
    unsigned g = (colorCode >> 8) % 256;
    unsigned b = colorCode % 256;

    return { float(r) / 255.f, float(g) / 255.f, float(b) / 255.f };
}

const glm::vec3 MERCURY_COLOR = FromRGB(0x5B71FF);
const glm::vec3 VENUS_COLOR = FromRGB(0xFFDB59);
const glm::vec3 EARTH_COLOR = FromRGB(0x168EFF);
const glm::vec3 MARS_COLOR = FromRGB(0xFF7A68);
const glm::vec3 JUPITER_COLOR = FromRGB(0x897AFF);
const glm::vec3 SATURN_COLOR = FromRGB(0x47FF81);
const glm::vec3 URANUS_COLOR = FromRGB(0xFFF463);
const glm::vec3 NEPTUNE_COLOR = FromRGB(0xFFF463);
const glm::vec3 PLUTO_COLOR = FromRGB(0xFFF463);
const glm::vec3 COMET_COLOR = FromRGB(0xFFFFFF);
}

CSolarSystem::CSolarSystem()
    : m_time(0)
    , m_timeSpeed(DEFAULT_TIME_SPEED)
    , m_viewScale(DEFAULT_SCALE)
{
    m_planets =
    {
        {CEllipticOrbit(0.387, 0.206, 1 / 0.241, GetRadians(7, 0)), MERCURY_COLOR, 1},
        {CEllipticOrbit(0.723, 0.007, 1 / 0.635, GetRadians(3, 24)), VENUS_COLOR, 3},
        {CEllipticOrbit(1.000, 0.017, 1 / 1.000, GetRadians(0)), EARTH_COLOR, 3},
        {CEllipticOrbit(1.524, 0.093, 1 / 1.881, GetRadians(1, 1)), MARS_COLOR, 2},
        {CEllipticOrbit(5.203, 0.048, 1 / 11.862, GetRadians(1, 18)), JUPITER_COLOR, 6},
        {CEllipticOrbit(6.539, 0.056, 1 / 20.658, GetRadians(2, 29)), SATURN_COLOR, 5},
        {CEllipticOrbit(19.190, 0.048, 1 / 84.800, GetRadians(0, 45)), URANUS_COLOR, 5},
        {CEllipticOrbit(30.081, 0.009, 1 / 154.232, GetRadians(1, 47)), NEPTUNE_COLOR, 5},
        {CEllipticOrbit(38.525, 0.249, 1 / 247.305, GetRadians(17, 9)), PLUTO_COLOR, 1},
        {CEllipticOrbit(17.800, 0.967, 1 / 75.300, GetRadians(162, 3)), COMET_COLOR, 0.5},
    };

}

void CSolarSystem::Update(float deltaTime)
{
    m_time += deltaTime * m_timeSpeed;
}

void CSolarSystem::Draw()
{
    DrawSun();
    DrawPlanets();
}

float CSolarSystem::GetViewScale() const
{
    return m_viewScale;
}

float CSolarSystem::GetTimeSpeed() const
{
    return m_timeSpeed;
}

unsigned CSolarSystem::GetYear() const
{
    return unsigned(m_time) + INITIAL_YEAR;
}

void CSolarSystem::ZoomIn()
{
    m_viewScale *= SCALE_FACTOR;
    if (m_viewScale > MAX_SCALE)
    {
        m_viewScale = MAX_SCALE;
    }
}

void CSolarSystem::ZoomOut()
{
    m_viewScale /= SCALE_FACTOR;
    if (m_viewScale < MIN_SCALE)
    {
        m_viewScale = MIN_SCALE;
    }
}

void CSolarSystem::SpeedupTime()
{
    m_timeSpeed = m_timeSpeed + TIME_ADJUSTMENT;
    if (m_timeSpeed > MAX_TIME_SPEED)
    {
        m_timeSpeed = MAX_TIME_SPEED;
    }

    m_timeSpeed = floorf(m_timeSpeed * 1000.f + 0.5f) / 1000.f;
}

void CSolarSystem::SlowdownTime()
{
    m_timeSpeed = m_timeSpeed - TIME_ADJUSTMENT;
    if (m_timeSpeed < MIN_TIME_SPEED)
    {
        m_timeSpeed = MIN_TIME_SPEED;
    }
    m_timeSpeed = floorf(m_timeSpeed * 1000.f + 0.5f) / 1000.f;
}

void CSolarSystem::DrawSun()
{
    glPointSize(2.f * sqrtf(m_viewScale));
    glBegin(GL_POINTS);
    {
        glColor3f(1, 1, 0);
        glVertex2d(0, 0);
    }
    glEnd();
}

void CSolarSystem::DrawOrbit(const CEllipticOrbit &orbit)
{
    glDisable(GL_POINT_SMOOTH);
    glPointSize(1);
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POINTS);
    {
        // const float period = 2 * M_PI / orbit.MeanMotion();
        const float a = float(orbit.LargeAxis());
        const float e = float(orbit.Eccentricity());
        const float step = float(2 * M_PI) / (a * m_viewScale);
        const float c = e * a;
        const float k = sqrtf(1.f - e * e);
        const float b = k * a;
        const float centerX = -c;
        const float centerY = 0;

        float time = 0;
        while (time < float(2 * M_PI))
        {
            glVertex2f(centerX + a * cosf(time), centerY + b * sinf(time));
            time += step;
        }
    }
    glEnd();
}

void CSolarSystem::DrawPlanets()
{
    for (PlanetInfo const& planet : m_planets)
    {
        DrawOrbit(planet.orbit);

        glColor3fv(glm::value_ptr(planet.color));

        glEnable(GL_POINT_SMOOTH);
        glPointSize(planet.size * sqrtf(m_viewScale) / 4.f);
        glBegin(GL_POINTS);
        {
            glm::vec2 pos = planet.orbit.PlanetPosition2D(double(m_time));
            glVertex2f(pos.x, pos.y);
        }
        glEnd();
    }
}
