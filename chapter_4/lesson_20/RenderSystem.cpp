#include "stdafx.h"
#include "RenderSystem.h"
#include "libscene/StaticModel3DRenderer.h"
#include "libscene/DrawUtils.h"
#include "includes/opengl-common.hpp"

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
    CStaticModel3DRenderer renderer;
    renderer.Use(m_planetProgram);
    renderer.SetProjectionMat4(projection);

    // Отключаем запись в буфер глубины для рисования объектов окружения,
    //  также устанавливаем модифицированную матрицу преобразования
    //  в координаты зрителя.
    glDepthMask(GL_FALSE);
    renderer.SetViewMat4(CDrawUtils::GetEnvironmentViewMat4(view));
    DoRenderPass(CMeshComponent::Environment, renderer);

    // Включаем обратно запись в буфер глубины для рисования объектов сцены.
    glDepthMask(GL_TRUE);
    renderer.SetViewMat4(view);
    DoRenderPass(CMeshComponent::Foreground, renderer);
}

void CRenderSystem::DoRenderPass(CMeshComponent::Category category, CStaticModel3DRenderer &renderer)
{
    for (const auto &entity : getEntities())
    {
        const auto &mesh = entity.getComponent<CMeshComponent>();
        // Пропускаем объекты, не попадающие в этот проход рисования.
        if (category != mesh.m_category)
        {
            continue;
        }

        const auto &transform = entity.getComponent<CTransformComponent>();
        renderer.SetWorldMat4(transform.ToMat4());
        renderer.Draw(*mesh.m_pModel);
    }
}
