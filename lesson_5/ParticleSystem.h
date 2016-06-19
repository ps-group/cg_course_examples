#pragma once

#include "MovableFlower.h"
#include <vector>
#include <memory>
#include <random>

// Создано на основе http://stackoverflow.com/questions/28618900
class CClampedNormalDistribution
{
    std::normal_distribution<float> m_distribution;
    float m_min = 0.f;
    float m_max = 0.f;
public:
    // m_distribution требует два параметра:
    // `mean`, т.е. медианное значение и одновременно мат. ожидание
    // `stddev`, т.е. стандартное отклонение (дисперсию)
    // мы выводим эти параметры из min/max.
    void param(float min, float max)
    {
        using param_type = std::normal_distribution<float>::param_type;
        const float mean = (min + max) / 2.f;
        const float stddev = (max - min) / 6.f;
        m_distribution.param(param_type(mean, stddev));
        m_min = min;
        m_max = max;
    }

    // Нормальное распределение выдаёт значения на всём диапазоне float
    // Но мы режем значения, выпадающие из диапазона [min, max]
    // Статистически, будет выброшено около 0.3% значений.
    float operator ()(std::mt19937 &random) {
        while (true) {
            float number = m_distribution(random);
            if (number >= m_min && number <= m_max)
                return number;
        }
    }
};

class CFlowerParticle : public CMovableFlower
{
public:
    // @param dt - разница во времени с предыдущим вызовом Advance.
    // @param acceleration - ускорение, действующее на частицу.
    void Advance(float dt, const glm::vec2 &acceleration);
    bool IsAlive()const;

    glm::vec2 GetVelocity() const;
    void SetVelocity(const glm::vec2 &GetVelocity);
    void SetLifetime(float lifetime);

private:
    glm::vec2 m_velocity;
    float m_lifetime = 0;
};

class CParticleEmitter
{
public:
    CParticleEmitter();

    // @param dt - разница во времени с предыдущим вызовом Advance.
    void Advance(float dt);
    bool IsEmitReady()const;
    std::unique_ptr<CFlowerParticle> Emit();

    void SetPosition(const glm::vec2 &value);
    void SetLifetimeRange(float minValue, float maxValue);
    void SetRadiusRange(float minValue, float maxValue);
    void SetPetalsCountRangle(int minValue, int maxValue);
    void SetEmitIntervalRange(float minValue, float maxValue);
    void SetSpeedRange(float minValue, float maxValue);
    void SetAngleRange(float minValue, float maxValue);

private:
    using linear_random_float = std::uniform_real_distribution<float>;
    using normal_random_float = CClampedNormalDistribution;
    using linear_random_int = std::uniform_int_distribution<int>;

    float m_elapsedSeconds = 0;
    float m_nextEmitTime = 0;
    glm::vec2 m_position;
    normal_random_float m_lifetimeRange;
    linear_random_float m_radiusRange;
    linear_random_int m_petalsCountRange;
    normal_random_float m_emitIntervalRange;
    normal_random_float m_speedRange;
    normal_random_float m_angleRange;
    std::mt19937 m_random;
};

class CParticleSystem
{
public:
    CParticleSystem();
    ~CParticleSystem();

    void SetEmitter(std::unique_ptr<CParticleEmitter> && pEmitter);

    // @param dt - разница во времени с предыдущим вызовом Advance.
    void Advance(float dt);
    void Draw();

private:
    std::unique_ptr<CParticleEmitter> m_pEmitter;
    std::vector<std::unique_ptr<CFlowerParticle>> m_flowers;
};
