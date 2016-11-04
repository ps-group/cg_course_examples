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
    CModel3DSharedPtr m_pModel;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
};
