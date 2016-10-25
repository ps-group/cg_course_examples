#include "stdafx.h"
#include "RenderSystem.h"
#include "PlanetRenderer.h"

CRenderSystem::CRenderSystem()
    : m_blackTexture(CTexture2D::no_texture_tag())
{
}

void CRenderSystem::SetupLight0(const glm::vec4 &position, const glm::vec4 &diffuse, const glm::vec4 &specular)
{
    CPlanetProgram::SLightSource sunlight;
    sunlight.position = position;
    sunlight.diffuse = diffuse;
    sunlight.specular = specular;
    m_planetProgram.SetLight0(sunlight);
}

void CRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    m_planetProgram.SetView(view);
    m_planetProgram.SetProjection(projection);

    CPlanetRenderer3D renderer(m_planetProgram);
    for (const auto &entity : getEntities())
    {
        auto &mesh = entity.getComponent<CStaticMeshComponent>();
        auto &transform = entity.getComponent<CTransformComponent>();

        m_planetProgram.BindDiffuseMap(GetTextureOrBlack(mesh.m_pDiffuseMap));
        m_planetProgram.BindSpecularMap(GetTextureOrBlack(mesh.m_pSpecularMap));
        m_planetProgram.BindEmissiveMap(GetTextureOrBlack(mesh.m_pEmissiveMap));
        m_planetProgram.SetModel(transform.ToMat4());
        m_planetProgram.UpdateModelViewProjection();
        mesh.m_pMesh->Draw(renderer);
    }
}

CTexture2D &CRenderSystem::GetTextureOrBlack(const CTexture2DSharedPtr &pTexture)
{
    if (pTexture)
    {
        return *pTexture;
    }
    return m_blackTexture;
}
