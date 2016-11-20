#include "stdafx.h"
#include "AnimationSystem.h"

CAnimationSystem::CAnimationSystem()
{
}

void CAnimationSystem::Update(float deltaSec)
{
    for (const anax::Entity &entity : getEntities())
    {
        auto &com = entity.getComponent<CAnimateComponent>();
        com.Update(deltaSec);
    }
}
