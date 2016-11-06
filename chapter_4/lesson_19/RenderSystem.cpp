#include "stdafx.h"
#include "RenderSystem.h"
#include "libscene/DrawUtils.h"
#include "includes/opengl-common.hpp"

CRenderSystem::CRenderSystem()
{
}

void CRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthFunc(GL_LESS);

    m_particleProgram.Use();
    m_particleProgram.GetUniform(UniformId::MATRIX_PROJECTION) = projection;
    for (const auto &entity : getEntities())
    {
        const auto &transform = entity.getComponent<CTransformComponent>();
        const glm::mat4 worldView = view * transform.ToMat4();
        m_particleProgram.GetUniform(UniformId::MATRIX_WORLDVIEW) = worldView;
        m_particleProgram.GetUniform(UniformId::TRANSFORM_SCALE) = transform.m_sizeScale;

        const auto &mesh = entity.getComponent<CParticleComponent>();
        mesh.m_pSystem->Draw(m_particleProgram, worldView);
    }

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}
