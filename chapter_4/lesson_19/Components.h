#pragma once
#include "MeshP3NT2.h"
#include "EllipticOrbit.h"
#include <functional>
#include <anax/Component.hpp>
#include <anax/Entity.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

class CStaticMeshComponent : public anax::Component
{
public:
    std::shared_ptr<CMeshP3NT2> m_pMesh;
    CTexture2DSharedPtr m_pDiffuseMap;
    CTexture2DSharedPtr m_pSpecularMap;
    CTexture2DSharedPtr m_pEmissiveMap;
    bool m_writesDepth = true;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
public:
    bool m_drawAroundCamera = false;
};

class CScriptComponent : public anax::Component
{
public:
    using UpdateFn = std::function<void(float dt, const anax::Entity &entity)>;

    UpdateFn m_onUpdate;
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
