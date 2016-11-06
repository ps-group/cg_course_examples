#pragma once
#include "Components.h"
#include "ParticleProgram.h"
#include <anax/System.hpp>

class CModel3DRenderer;

class CParticleRenderSystem
        : public anax::System<anax::Requires<CParticleSystemComponent, CTransformComponent>>
{
public:
    CParticleRenderSystem();

    void Render(const glm::mat4 &view, const glm::mat4 &projection);

private:
    std::vector<anax::Entity> GetEntitesSortedByDepth(const glm::mat4 &view)const;

    CParticleProgram m_particleProgram;
};
