#include "SkeletalAnimator.h"
#include <boost/range/algorithm/find_if.hpp>
#include <iostream>

// для перегруженной glm::lerp, принимающей glm::vec3
#include <glm/gtx/compatibility.hpp>

namespace
{
template <class T>
using Keyframe = CNodeAnimation::Keyframe<T>;

// Возвращает значение, интерполированное между двумя ключевыми кадрами.
template <class T>
T GetInterpolatedValue(const std::vector<Keyframe<T>> &keyframes, double time)
{
    // Ищем первый ключевой кадр (keyframe),
    //  который должен наступить позднее текущей фазы анимации.
    size_t ki = 0;
    for (; ki < keyframes.size(); ++ki)
    {
        if (keyframes[ki].time >= time)
        {
            break;
        }
    }

    // Если анимация находится до первого ключевого кадра или после последнего,
    //  то просто возвращаем крайнее значение.
    if (ki == 0)
    {
        return keyframes.front().value;
    }
    if (ki == keyframes.size())
    {
        return keyframes.back().value;
    }

    // Если анимация находится между двумя ключевыми кадрами,
    //  интерполируем их значения.
    // Функция glm::lerp перегружена для скаляров,
    //  для векторов из 2-4 значений и для квантерионов.
    const auto &a = keyframes[ki - 1];
    const auto &b = keyframes[ki];
    const double weight = (time - a.time) / (b.time - a.time);

    return glm::lerp(a.value, b.value, float(weight));
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
