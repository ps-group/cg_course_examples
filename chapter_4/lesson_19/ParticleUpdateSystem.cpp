#include "stdafx.h"
#include "ParticleUpdateSystem.h"

void CParticleUpdateSystem::Update(float deltaSec)
{
    for (auto &entity : getEntities())
    {
        auto &particleCom = entity.getComponent<CParticleSystemComponent>();
        particleCom.m_pSystem->Advance(deltaSec);
    }
}
