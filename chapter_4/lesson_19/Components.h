#pragma once
#include "MeshP3NT2.h"
#include "EllipticOrbit.h"
#include <anax/Component.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

class CStaticMeshComponent : public anax::Component
{
public:
    std::shared_ptr<CMeshP3NT2> m_pMesh;
    CTexture2DUniquePtr m_pDiffuseMap;
    CTexture2DUniquePtr m_pSpecularMap;
    CTexture2DUniquePtr m_pEmissiveMap;
};

class CTransformComponent
        : public anax::Component
        , public CTransform3D
{
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
