#pragma once

#include <random>
#include "includes/glm-common.hpp"

class CParticle
{
public:
    CParticle(const glm::vec3 &position,
              const glm::vec3 &velocity,
              float lifetime);

    void Advance(float deltaSeconds, const glm::vec3 &acceleration);

    glm::vec3 GetPosition()const;
    bool IsAlive()const;

private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_lifetime = 0;
};

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

// Класс источника частиц, создающего частицы,
//  вылетающие из заданного в заданном направлении.
// Случайным вариациям поддаются:
//  - дистанция от источника частиц, на которой появляется частица
//  - отклонение направления частицы от направления источника
//  - скорость частицы
//  - время жизни частицы
//  - интервал между генерацией двух частиц
class CParticleEmitter
{
public:
    CParticleEmitter();

    // @param dt - разница во времени с предыдущим вызовом Advance.
    void Advance(float dt);
    bool IsEmitReady()const;
    CParticle Emit();

    // Задаёт центр источника частиц.
    void SetPosition(const glm::vec3 &value);

    // Задаёт разброс расстояния от места появления частицы до центра источника.
    void SetDistanceRange(float minValue, float maxValue);

    // Задаёт направление вылета частиц.
    void SetDirection(const glm::vec3 &value);

    // Задаёт максимальный угол отклонения направления частицы
    //  от основного направления вылета частиц.
    void SetMaxDeviationAngle(float value);

    // Задаёт разброс времени жизни частиц.
    void SetLifetimeRange(float minValue, float maxValue);

    // Задаёт разброс времени между вылетом двух частиц.
    void SetEmitIntervalRange(float minValue, float maxValue);

    // Задаёт разброс скорости частицы.
    void SetSpeedRange(float minValue, float maxValue);

private:
    using linear_random_float = std::uniform_real_distribution<float>;
    using normal_random_float = CClampedNormalDistribution;

    glm::vec3 MakeRandomDirection();

    float m_elapsedSeconds = 0;
    float m_nextEmitTime = 0;
    glm::vec3 m_position;
    glm::vec3 m_direction = glm::vec3(0, 1, 0);
    linear_random_float m_distanceRange;
    linear_random_float m_deviationAngleRange;
    normal_random_float m_lifetimeRange;
    normal_random_float m_emitIntervalRange;
    normal_random_float m_speedRange;
    std::mt19937 m_random;
};
