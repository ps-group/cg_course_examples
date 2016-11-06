#include "stdafx.h"
#include "UpdateSystem.h"

void CUpdateSystem::Update(float deltaSec)
{
    for (auto &entity : getEntities())
    {
        auto &particleCom = entity.getComponent<CParticleComponent>();
        particleCom.m_pSystem->Advance(deltaSec);
    }
}
