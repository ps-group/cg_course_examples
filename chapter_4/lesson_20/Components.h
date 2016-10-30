#pragma once
#include "libchapter4/ComplexMesh.h"
#include "libchapter4/Transform.h"
#include <anax/Component.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

class CMeshComponent : public anax::Component
{
public:
    CComplexMeshSharedPtr m_pMesh;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
};
