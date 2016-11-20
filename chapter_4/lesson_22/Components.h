#pragma once
#include "libscene/SkeletalModel3D.h"
#include "libscene/SkeletalAnimator.h"
#include "libgeometry/Transform.h"
#include <anax/Component.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

class CMeshComponent
        : public anax::Component
{
public:
    enum Category
    {
        // Объект заднего плана, сливающийся с окружением.
        Environment,
        // Объект переднего плана.
        Foreground,
    };

    Category m_category;
    CSkeletalModel3DPtr m_pModel;
};

class CAnimateComponent
        : public anax::Component
        , public CSkeletalAnimator
{
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
};
