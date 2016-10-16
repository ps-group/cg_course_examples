#include "stdafx.h"
#include "KeplerLawSystem.h"
#include "TimeController.h"
#include <glm/gtx/quaternion.hpp>

static const float ASTRONOMICAL_UNIT_SIZE = 30.f;

CKeplerLawSystem::CKeplerLawSystem(ITimeController &controller)
    : m_timeController(controller)
{
}

void CKeplerLawSystem::Update()
{
    const double time = m_timeController.GetSpaceTime();
    for (const auto &entity : getEntities())
    {
//        auto &body = entity.getComponent<CSpaceBodyComponent>();
        auto &transform = entity.getComponent<CTransformComponent>();
        auto &orbit = entity.getComponent<CEllipticOrbitComponent>();

        const glm::vec2 position2D = orbit.PlanetPosition2D(time);
        const glm::vec3 position3D = ASTRONOMICAL_UNIT_SIZE
                * glm::vec3{ position2D.x, 0.f, position2D.y };

        transform.m_translate = position3D;
    }
}
