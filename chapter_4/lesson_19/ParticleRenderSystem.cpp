#include "stdafx.h"
#include "ParticleRenderSystem.h"
#include "libscene/DrawUtils.h"
#include "includes/opengl-common.hpp"

CParticleRenderSystem::CParticleRenderSystem()
{
}

void CParticleRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

    m_particleProgram.Use();
    m_particleProgram.GetUniform(UniformId::MATRIX_PROJECTION) = projection;
    CVertexAttribute positionAttr = m_particleProgram.GetAttribute(AttributeId::INSTANCE_POSITION);
    positionAttr.EnablePointer();
    positionAttr.SetDivisor(1);
    CVertexAttribute texCoordAttr = m_particleProgram.GetAttribute(AttributeId::TEX_COORD_UV);
    texCoordAttr.EnablePointer();

    const std::vector<anax::Entity> entities = GetEntitesSortedByDepth(view);
    for (const auto &entity : entities)
    {
        const auto &transform = entity.getComponent<CTransformComponent>();
        const glm::mat4 worldView = view * transform.ToMat4();
        m_particleProgram.GetUniform(UniformId::MATRIX_WORLDVIEW) = worldView;
        m_particleProgram.GetUniform(UniformId::TRANSFORM_SCALE) = transform.m_sizeScale;

        const auto &mesh = entity.getComponent<CParticleSystemComponent>();
        mesh.m_pSystem->Draw(m_particleProgram, worldView);
    }

    texCoordAttr.DisablePointer();
    positionAttr.SetDivisor(0);
    positionAttr.DisablePointer();
	glDisable(GL_BLEND);
	glDisable(GL_BLEND);
}

std::vector<anax::Entity> CParticleRenderSystem::GetEntitesSortedByDepth(const glm::mat4 &view) const
{
    std::vector<anax::Entity> result = getEntities();
    std::sort(result.begin(), result.end(), [&](const anax::Entity &a, const anax::Entity &b) {
        const auto &transformA = a.getComponent<CTransformComponent>();
        const auto &transformB = b.getComponent<CTransformComponent>();
        const glm::vec3 posA = CDrawUtils::TransformPoint(transformA.m_position, view);
        const glm::vec3 posB = CDrawUtils::TransformPoint(transformB.m_position, view);

        return posA.z < posB.z;
    });

    return result;
}
