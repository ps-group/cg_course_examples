#pragma once
#include "SkeletalModel3D.h"

/// Класс отвечает за обновление транформаций узлов с учётом текущей анимации
///  согласно правилам скелетной анимации.
class CSkeletalAnimator
{
public:
    // Устанавливает модель, на которую воздействует аниматор.
    void SetModel(const CSkeletalModel3DPtr &pModel);

    // Включает анимацию с заданным именем,
    //  бросает исключение, если анимации нет.
    void SetAnimationName(const std::string &name);

    // Включает анимацию с заданным номером.
    void SetAnimationNo(unsigned animationNo);

    // Обновляет состояние модели согласно прошедшему времени.
    void Update(float deltaSec);

private:
    const std::vector<CModelAnimation> &GetAnimations()const;
    void ApplyNodeAnimation(const CNodeAnimation &anim);

    unsigned m_animationNo = 0;
    double m_animationPhase = 0;
    CSkeletalModel3DPtr m_pModel;
};
