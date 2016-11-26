#define _USE_MATH_DEFINES
#include "ParticleEmitter.h"

// Расширение GLM_GTX_rotate_vector
// см. http://glm.g-truc.net/0.9.3/api/a00199.html
#include <glm/gtx/rotate_vector.hpp>

using glm::vec3;

CParticle::CParticle(const vec3 &position, const vec3 &velocity, float lifetime)
    : m_position(position)
    , m_velocity(velocity)
    , m_lifetime(lifetime)
{
}

void CParticle::Advance(float deltaSeconds, const glm::vec3 &acceleration)
{
    m_lifetime -= deltaSeconds;
    m_velocity += acceleration * deltaSeconds;
    m_position += m_velocity * deltaSeconds;
}

glm::vec3 CParticle::GetPosition() const
{
    return m_position;
}

bool CParticle::IsAlive() const
{
    return (m_lifetime > std::numeric_limits<float>::epsilon());
}

CParticleEmitter::CParticleEmitter()
{
    std::random_device rd;
    m_random.seed(rd());
}

void CParticleEmitter::Advance(float dt)
{
    m_elapsedSeconds += dt;
}

bool CParticleEmitter::IsEmitReady() const
{
    return m_elapsedSeconds > m_nextEmitTime;
}

CParticle CParticleEmitter::Emit()
{
    // Увеличиваем время следующего вылета частицы.
    m_nextEmitTime += m_emitIntervalRange(m_random);

    const vec3 direction = MakeRandomDirection();
    const vec3 position = m_position + direction * m_distanceRange(m_random);
    const vec3 velocity = direction * m_speedRange(m_random);
    const float lifetime = m_lifetimeRange(m_random);

    return CParticle(position, velocity, lifetime);
}

void CParticleEmitter::SetPosition(const glm::vec3 &value)
{
    m_position = value;
}

void CParticleEmitter::SetDistanceRange(float minValue, float maxValue)
{
    m_distanceRange.param(linear_random_float::param_type(minValue, maxValue));
}

void CParticleEmitter::SetDirection(const glm::vec3 &value)
{
    m_direction = glm::normalize(value);
}

void CParticleEmitter::SetMaxDeviationAngle(float value)
{
    m_deviationAngleRange.param(linear_random_float::param_type(0.f, value));
}

void CParticleEmitter::SetLifetimeRange(float minValue, float maxValue)
{
    m_lifetimeRange.param(minValue, maxValue);
}

void CParticleEmitter::SetEmitIntervalRange(float minValue, float maxValue)
{
    m_emitIntervalRange.param(minValue, maxValue);
}

void CParticleEmitter::SetSpeedRange(float minValue, float maxValue)
{
    m_speedRange.param(minValue, maxValue);
}

glm::vec3 CParticleEmitter::MakeRandomDirection()
{
    vec3 dir = m_direction;

    // Данный вектор будет ортогонален вектору dir, что несложно проверить,
    //  вычислив векторное произведение dir и normal.
    vec3 normal = glm::normalize(vec3(dir.y + dir.z, -dir.x, -dir.x));

    // Поворачиваем normal на произвольный угол (в диапазоне -M_PI..M_PI)
    //  вокруг вектора dir, чтобы получить случайную ось вращения.
    linear_random_float distribution(float(-M_PI), float(M_PI));
    normal = glm::rotate(normal, distribution(m_random), dir);

    // Поворачиваем dir вокруг повёрнутого normal (по-прежнему ортогонального)
    //  на случайный угол, ограниченный максимальным углом отклонения.
    const float deviationAngle = m_deviationAngleRange(m_random);
    dir = glm::rotate(dir, deviationAngle, normal);

    return dir;
}
