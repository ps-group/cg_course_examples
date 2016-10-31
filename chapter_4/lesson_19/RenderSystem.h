#pragma once
#include "Components.h"
#include "PlanetProgram.h"
#include <anax/System.hpp>

class CPlanetRenderer3D;

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
    enum DrawPass
    {
        BackgroundObjects,
        ForegroundObjects,
        // TransparentObjects,
    };

    DrawPass SelectDrawPass(const anax::Entity &entity);
    void RenderImpl(DrawPass pass, CPlanetRenderer3D &renderer, const glm::mat4 &view);

    CTexture2D &GetTextureOrBlack(const CTexture2DSharedPtr &pTexture);
    void ApplyMaterial(const CStaticMeshComponent &mesh);
    void ApplyTransform(const CTransformComponent &transform, const glm::mat4 &view);

    CPlanetProgram m_planetProgram;
    CTexture2D m_blackTexture;
};
