#pragma once
#include "libscene/ParticleSystem.h"
#include "libgeometry/Transform.h"
#include <anax/Component.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

class CParticleComponent
        : public anax::Component
{
public:
    std::shared_ptr<CParticleSystem> m_pSystem;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
};
