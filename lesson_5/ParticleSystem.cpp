#include "stdafx.h"
#include "ParticleSystem.h"
#include <limits>

namespace
{
const glm::vec2 GRAVITY = {0.f, 9.8f};
}

void CFlowerParticle::Advance(float dt, const glm::vec2 &acceleration)
{
    m_lifetime -= dt;
    m_velocity += dt * acceleration;
    SetPosition(m_velocity + GetPosition());
}

bool CFlowerParticle::IsAlive() const
{
    const float epsilon = std::numeric_limits<float>::epsilon();
    return m_lifetime > epsilon;
}

glm::vec2 CFlowerParticle::GetVelocity() const
{
    return m_velocity;
}

void CFlowerParticle::SetVelocity(const glm::vec2 &velocity)
{
    m_velocity = velocity;
}

void CFlowerParticle::SetLifetime(float lifetime)
{
    m_lifetime = lifetime;
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

std::unique_ptr<CFlowerParticle> CParticleEmitter::Emit()
{
    const glm::vec3 LIGHT_YELLOW = { 1.f, 1.f, 0.5f};
    const glm::vec3 ORANGE = {1.f, 0.5f, 0.f};

    auto pFlower = std::make_unique<CFlowerParticle>();

    // Определяем время появления следующей частицы.
    m_elapsedSeconds -= m_nextEmitTime;
    m_nextEmitTime = m_emitIntervalRange(m_random);

    const float speed = m_speedRange(m_random);
    const float angle = m_angleRange(m_random);
    const glm::vec2 velocity = { speed * sinf(angle), speed * cosf(angle) };
    const float maxRadius = m_radiusRange(m_random);
    const int petalsCount = m_petalsCountRange(m_random);

    pFlower->SetLifetime(m_lifetimeRange(m_random));
    pFlower->SetVelocity(velocity);
    pFlower->SetCenterColor(LIGHT_YELLOW);
    pFlower->SetPetalColor(ORANGE);
    pFlower->SetPosition(m_position);
    pFlower->SetupShape(0.3f * maxRadius, maxRadius, petalsCount);

    return pFlower;
}

void CParticleEmitter::SetPosition(const glm::vec2 &value)
{
    m_position = value;
}

void CParticleEmitter::SetLifetimeRange(float minValue, float maxValue)
{
    m_lifetimeRange.param(minValue, maxValue);
}

void CParticleEmitter::SetRadiusRange(float minValue, float maxValue)
{
    m_radiusRange.param(linear_random_float::param_type(minValue, maxValue));
}

void CParticleEmitter::SetPetalsCountRangle(int minValue, int maxValue)
{
    m_petalsCountRange.param(linear_random_int::param_type(minValue, maxValue));
}

void CParticleEmitter::SetEmitIntervalRange(float minValue, float maxValue)
{
    m_emitIntervalRange.param(minValue, maxValue);
}

void CParticleEmitter::SetSpeedRange(float minValue, float maxValue)
{
    m_speedRange.param(minValue, maxValue);
}

void CParticleEmitter::SetAngleRange(float minValue, float maxValue)
{
    m_angleRange.param(minValue, maxValue);
}

CParticleSystem::CParticleSystem() = default;
CParticleSystem::~CParticleSystem() = default;

void CParticleSystem::SetEmitter(std::unique_ptr<CParticleEmitter> &&pEmitter)
{
    m_pEmitter = std::move(pEmitter);
}

void CParticleSystem::Advance(float dt)
{
    // Генерируем новые частицы
    m_pEmitter->Advance(dt);
    // За 1 кадр может появиться несколько новых частиц.
    while (m_pEmitter->IsEmitReady())
    {
        m_flowers.emplace_back(m_pEmitter->Emit());
    }
    // Продвигаем время жизни всех цветов.
    for (const auto &pFlower : m_flowers)
    {
        pFlower->Advance(dt, GRAVITY);
    }
    // Удаляем "умершие" цветы.
    auto newEnd = std::remove_if(m_flowers.begin(), m_flowers.end(), [](const auto &pFlower) {
        return !pFlower->IsAlive();
    });
    m_flowers.erase(newEnd, m_flowers.end());
}

void CParticleSystem::Draw()
{
    for (const auto &pFlower : m_flowers)
    {
        pFlower->Draw();
    }
}
