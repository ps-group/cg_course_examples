#pragma once
#include "libscene/Model3D.h"
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
    CModel3DSharedPtr m_pModel;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
};
