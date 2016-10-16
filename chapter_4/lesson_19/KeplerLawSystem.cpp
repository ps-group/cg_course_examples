#include "stdafx.h"
#include "KeplerLawSystem.h"

const double CKeplerLawSystem::DEFAULT_TIME_SPEED = 0.02;
static const float ASTRONOMICAL_UNIT_SIZE = 30.f;

void CKeplerLawSystem::Update(float deltaSeconds)
{
    m_time += double(deltaSeconds) * m_timeSpeed;

    for (const auto &entity : getEntities())
    {
//        auto &body = entity.getComponent<CSpaceBodyComponent>();
        auto &transform = entity.getComponent<CTransformComponent>();
        auto &orbit = entity.getComponent<CEllipticOrbitComponent>();

        const glm::vec2 position = orbit.PlanetPosition2D(m_time);
        transform.m_position = ASTRONOMICAL_UNIT_SIZE
                * glm::vec3{ position.x, 0.f, position.y };
    }
}
