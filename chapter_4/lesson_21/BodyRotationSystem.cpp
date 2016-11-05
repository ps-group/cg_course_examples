#include "stdafx.h"
#include "BodyRotationSystem.h"
#include "TimeController.h"
#include "Scale.h"

CBodyRotationSystem::CBodyRotationSystem(ITimeController &controller)
    : m_timeContoller(controller)
{
}

void CBodyRotationSystem::Update()
{
    const double time = m_timeContoller.GetSpaceTime();
    for (const auto &entity : getEntities())
    {
        auto &body = entity.getComponent<CSpaceBodyComponent>();
        auto &transform = entity.getComponent<CTransformComponent>();

        const double rotationSpeed = 1.0 / double(body.m_dayDuration);
        const float angle = float(fmod(rotationSpeed * time, 2 * M_PI));
        const glm::quat orientation = glm::angleAxis(angle, body.m_rotationAxis);

        const float scale = scale::GetPlanetSizeInAU(body.m_bodySize);

        transform.m_sizeScale = glm::vec3(scale);
        transform.m_orientation = orientation;
    }
}
