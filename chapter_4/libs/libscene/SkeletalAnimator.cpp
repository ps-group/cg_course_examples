#include "SkeletalAnimator.h"
#include <boost/range/algorithm/find_if.hpp>
#include <iostream>

// для перегруженной glm::lerp, принимающей glm::vec3
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::quat;
using glm::vec3;

namespace
{
template <class T>
using Keyframe = CNodeAnimation::Keyframe<T>;

// Выполняет сферическую линейную интерполяцию (slerp) двух кватернионов.
quat Interpolate(const quat &a, const quat &b, float weight)
{
    return glm::slerp(a, b, weight);
}

// Выполняет линейную интерполяцию (смешивание) двух векторов.
vec3 Interpolate(const vec3 &a, const vec3 &b, float weight)
{
    return glm::mix(a, b, weight);
}

// Возвращает значение, интерполированное между двумя ключевыми кадрами.
template <class T>
T GetInterpolatedValue(const std::vector<Keyframe<T>> &keyframes, double phase)
{
    // Обрабатываем вырожденный случай (массив из 1-го элемента).
    if (keyframes.size() <= 1)
    {
        return keyframes.at(0).value;
    }

    // Ищем ближайший ключевой кадр (keyframe),
    //  который анимации предстоит пройти.
    size_t ki = 0;
    for (; ki < keyframes.size(); ++ki)
    {
        if (keyframes[ki].time >= phase)
        {
            break;
        }
    }

    // Если анимация не достигла первого ключевого кадра, используем его.
    if (ki == 0)
    {
        return keyframes.front().value;
    }
    // Если анимация преодолела последний ключевой кадр, используем его.
    if (ki == keyframes.size())
    {
        return keyframes.back().value;
    }

    // Если анимация находится между двумя ключевыми кадрами,
    //  пройденным и предстоящим, и мы интерполируем их значения.
    // Функция Interpolate перегружена для vec3 и для quat.
    const auto &a = keyframes[ki - 1];
    const auto &b = keyframes[ki];
    const double weight = (phase - a.time) / (b.time - a.time);

    // Вес будет в диапазоне [0..1], что даёт усреднённое значение между A и B.
    assert(weight > -0.01f && weight < 1.01f);

    return Interpolate(a.value, b.value, float(weight));
}
}

void CSkeletalAnimator::SetModel(const CSkeletalModel3DPtr &pModel)
{
    m_pModel = pModel;
}

void CSkeletalAnimator::SetAnimationName(const std::string &name)
{
    if (!m_pModel)
    {
        throw std::runtime_error("No model set for animator");
    }

    const auto &animations = GetAnimations();
    auto it = boost::find_if(animations, [&](const CModelAnimation &anim) {
        return anim.m_name == name;
    });
    if (it == animations.end())
    {
        throw std::runtime_error("No animation '" + name + "' in model");
    }

    m_animationNo = unsigned(it - animations.begin());
}

void CSkeletalAnimator::SetAnimationNo(unsigned animationNo)
{
    m_animationNo = animationNo;
}

void CSkeletalAnimator::Update(float deltaSec)
{
    if (!m_pModel)
    {
        return;
    }

    const CModelAnimation &anim = GetAnimations().at(m_animationNo);

    // Используем деление с остатком на период текущей анимации,
    //  чтобы определить фазу анимации
    //  (т.е. число ticks, проигранных с начала анимации).
    const double deltaTicks = double(deltaSec) * anim.m_ticksPerSecond;
    m_animationPhase = fmod(m_animationPhase + deltaTicks, anim.m_duration);

    // Применяем текущую фазу анимации ко всем узлам.
    for (const CNodeAnimation &nodeAnim : anim.m_channels)
    {
        ApplyNodeAnimation(nodeAnim);
    }
}

const std::vector<CModelAnimation> &CSkeletalAnimator::GetAnimations() const
{
    return m_pModel->m_animations;
}

void CSkeletalAnimator::ApplyNodeAnimation(const CNodeAnimation &anim)
{
    CTransform3D &transform = anim.m_pNode->m_transform;
    transform.m_sizeScale = GetInterpolatedValue(anim.m_scaleKeyframes, m_animationPhase);
    transform.m_position = GetInterpolatedValue(anim.m_positionKeyframes, m_animationPhase);
    transform.m_orientation = GetInterpolatedValue(anim.m_rotationKeyframes, m_animationPhase);
}
