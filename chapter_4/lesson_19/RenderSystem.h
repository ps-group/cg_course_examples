#pragma once
#include "Components.h"
#include "ParticleProgram.h"
#include <anax/System.hpp>

class CModel3DRenderer;

class CRenderSystem
        : public anax::System<anax::Requires<CParticleComponent, CTransformComponent>>
{
public:
    CRenderSystem();

    void Render(const glm::mat4 &view, const glm::mat4 &projection);

private:
    CParticleProgram m_particleProgram;
};
