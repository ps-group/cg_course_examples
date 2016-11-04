#include "stdafx.h"
#include "RenderSystem.h"
#include "libscene/Model3DRenderer.h"

CRenderSystem::CRenderSystem()
{
}

void CRenderSystem::SetupLight0(const glm::vec4 &position, const glm::vec4 &diffuse, const glm::vec4 &specular)
{
    m_planetProgram.Use();
    m_planetProgram.GetUniform(UniformId::LIGHT_POSITION) = position;
    m_planetProgram.GetUniform(UniformId::LIGHT_DIFFUSE) = diffuse;
    m_planetProgram.GetUniform(UniformId::LIGHT_SPECULAR) = specular;
}

void CRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    CModel3DRenderer renderer;
    renderer.Use(m_planetProgram);
    renderer.SetProjectionMat4(projection);
    renderer.SetViewMat4(view);
    for (const auto &entity : getEntities())
    {
        auto &transform = entity.getComponent<CTransformComponent>();
        renderer.SetWorldMat4(transform.ToMat4());
        auto &mesh = entity.getComponent<CMeshComponent>();
        renderer.Draw(*mesh.m_pModel);
    }
}
