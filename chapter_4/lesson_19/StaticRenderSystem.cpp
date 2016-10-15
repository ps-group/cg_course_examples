#include "stdafx.h"
#include "StaticRenderSystem.h"
#include "StaticRenderer.h"

CStaticRenderSystem::CStaticRenderSystem()
{
    const glm::vec4 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f, 0.f};
    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
    const glm::vec4 FADED_WHITE_RGBA = {0.3f, 0.3f, 0.3f, 1.0f};

    CStaticProgramContext::SLightSource sunlight;
    sunlight.position = SUNLIGHT_DIRECTION;
    sunlight.diffuse = WHITE_RGBA;
    sunlight.specular = FADED_WHITE_RGBA;
    m_context.SetLight0(sunlight);
}

void CStaticRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    m_context.SetView(view);
    m_context.SetProjection(projection);

    CStaticRenderer3D renderer(m_context);
    for (const auto &entity : getEntities())
    {
        auto &mesh = entity.getComponent<CStaticMeshComponent>();
        m_context.BindDiffuseMap(*(mesh.m_pDiffuseMap));
        m_context.BindSpecularMap(*(mesh.m_pSpecularMap));
        m_context.SetModel(mesh.m_worldTransform);
        m_context.UpdateModelViewProjection();
        mesh.m_pMesh->Draw(renderer);
    }
}
