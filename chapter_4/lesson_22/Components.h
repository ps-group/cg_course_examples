#pragma once
#include "libscene/SkeletalModel3D.h"
#include "libscene/StaticModel3D.h"
#include "libscene/SkeletalAnimator.h"
#include "libgeometry/Transform.h"
#include <anax/Component.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include <boost/variant.hpp>


class CRenderableComponent
        : public anax::Component
{
public:
    struct EnvironmentObject
    {
        CStaticModel3DPtr m_pModel;
    };

    struct ForegroundObject
    {
        CSkeletalModel3DPtr m_pModel;
    };

    using Object = boost::variant<
            EnvironmentObject,
            ForegroundObject
        >;

    Object m_object;
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
