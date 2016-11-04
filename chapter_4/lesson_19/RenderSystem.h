#pragma once
#include "Components.h"
#include "PlanetProgram.h"
#include <anax/System.hpp>

class CPlanetRenderer3D;

class CRenderSystem
        : public anax::System<anax::Requires<CMeshComponent, CTransformComponent>>
{
public:
    CRenderSystem();

    void SetupLight0(const glm::vec4 &position,
                     const glm::vec4 &diffuse,
                     const glm::vec4 &specular);
    void Render(const glm::mat4 &view, const glm::mat4 &projection);

private:
    struct LightSource
    {
        glm::vec4 m_position;
        glm::vec4 m_diffuse;
        glm::vec4 m_specular;
    };

    void DoRenderPass(CMeshComponent::Category category, CPlanetRenderer3D &renderer);

    CPlanetProgram m_planetProgram;
    LightSource m_light0;
};
