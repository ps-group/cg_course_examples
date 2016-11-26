#pragma once
#include "libscene/StaticModel3D.h"
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
    CStaticModel3DPtr m_pModel;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
};
