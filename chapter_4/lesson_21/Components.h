#pragma once
#include "EllipticOrbit.h"
#include "libscene/ParticleSystem.h"
#include "libscene/Tesselator.h"
#include "libgeometry/Transform.h"
#include <functional>
#include <anax/Component.hpp>
#include <anax/Entity.hpp>

class CMeshComponent : public anax::Component
{
public:
    enum Category
    {
        // Объект заднего плана, сливающийся с окружением.
        Environment,
        // Объект переднего плана.
        Foreground,
    };

    CStaticGeometry m_geometry;
    CTexture2DSharedPtr m_pDiffuse;
    CTexture2DSharedPtr m_pSpecular;
    CTexture2DSharedPtr m_pEmissive;
    Category m_category = Category::Foreground;
};

class CParticleSystemComponent
        : public anax::Component
{
public:
    std::shared_ptr<CParticleSystem> m_pSystem;
    float m_particleScale = 1.f;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
public:
};

class CSpaceBodyComponent : public anax::Component
{
public:
    float m_dayDuration = 0;
    float m_bodySize = 0;
    glm::vec3 m_rotationAxis;
    std::string m_name;
};

class CEllipticOrbitComponent
        : public anax::Component
        , public CEllipticOrbit
{
public:
    CEllipticOrbitComponent(
        double const& largeAxis,     // большая полуось эллипса
        double const& eccentricity,  // эксцентриситет орбиты
        double const& meanMotion,    // среднее движение (градуcов за единицу времени)
        double const& periapsisEpoch // начальная эпоха прохождения через перигелий
        )
        : CEllipticOrbit(largeAxis, eccentricity, meanMotion, periapsisEpoch)
    {
    }

    std::string m_ownerName;
};
