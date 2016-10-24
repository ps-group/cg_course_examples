#pragma once
#include "Components.h"
#include "PlanetProgram.h"
#include <anax/System.hpp>

class CRenderSystem
        : public anax::System<anax::Requires<CStaticMeshComponent, CTransformComponent>>
{
public:
    CRenderSystem();

    void SetupLight0(const glm::vec4 &position,
                     const glm::vec4 &diffuse,
                     const glm::vec4 &specular);
    void Render(const glm::mat4 &view, const glm::mat4 &projection);

private:
    CTexture2D &GetTextureOrBlack(const CTexture2DSharedPtr &pTexture);

    CPlanetProgram m_planetProgram;
    CTexture2D m_blackTexture;
};
