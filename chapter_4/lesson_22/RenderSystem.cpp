#include "stdafx.h"
#include "RenderSystem.h"
#include "libscene/SkeletalModel3DRenderer.h"
#include "libscene/StaticModel3DRenderer.h"
#include "libscene/DrawUtils.h"
#include "includes/opengl-common.hpp"

namespace
{
void SetupProgramLight0(IProgramAdapter &adapter,
                 const glm::vec4 &position,
                 const glm::vec4 &diffuse,
                 const glm::vec4 &specular)
{
    adapter.Use();
    adapter.GetUniform(UniformId::LIGHT_POSITION) = position;
    adapter.GetUniform(UniformId::LIGHT_DIFFUSE) = diffuse;
    adapter.GetUniform(UniformId::LIGHT_SPECULAR) = specular;
}
}

CRenderSystem::CRenderSystem()
{
}

void CRenderSystem::SetupLight0(const glm::vec4 &position, const glm::vec4 &diffuse, const glm::vec4 &specular)
{
    SetupProgramLight0(m_skeletalProgram, position, diffuse, specular);
    SetupProgramLight0(m_phongProgram, position, diffuse, specular);
}

void CRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    RenderEnvironment(view, projection);
    RenderForeground(view, projection);
}

void CRenderSystem::RenderEnvironment(const glm::mat4 &view, const glm::mat4 &projection)
{
    CStaticModel3DRenderer renderer;
    renderer.Use(m_phongProgram);
    renderer.SetProjectionMat4(projection);

    // Отключаем запись в буфер глубины для рисования объектов окружения,
    //  также устанавливаем модифицированную матрицу преобразования
    //  в координаты зрителя.
    glDepthMask(GL_FALSE);
    renderer.SetViewMat4(CDrawUtils::GetEnvironmentViewMat4(view));

    for (const auto &entity : getEntities())
    {
        const auto &mesh = entity.getComponent<CRenderableComponent>();
        // Пропускаем объекты, не попадающие в этот проход рисования.
        if (mesh.m_object.type() != typeid(CRenderableComponent::EnvironmentObject))
        {
            continue;
        }

        const auto &object = boost::get<CRenderableComponent::EnvironmentObject>(mesh.m_object);
        const auto &transform = entity.getComponent<CTransformComponent>();
        renderer.SetWorldMat4(transform.ToMat4());
        renderer.Draw(*object.m_pModel);
    }
}

void CRenderSystem::RenderForeground(const glm::mat4 &view, const glm::mat4 &projection)
{
    CSkeletalModel3DRenderer renderer;
    renderer.Use(m_skeletalProgram);
    renderer.SetProjectionMat4(projection);
    renderer.SetViewMat4(view);

    // Включаем запись в буфер глубины для рисования объектов сцены.
    glDepthMask(GL_TRUE);

    for (const auto &entity : getEntities())
    {
        const auto &mesh = entity.getComponent<CRenderableComponent>();
        // Пропускаем объекты, не попадающие в этот проход рисования.
        if (mesh.m_object.type() != typeid(CRenderableComponent::ForegroundObject))
        {
            continue;
        }

        const auto &object = boost::get<CRenderableComponent::ForegroundObject>(mesh.m_object);
        const auto &transform = entity.getComponent<CTransformComponent>();
        renderer.SetWorldMat4(transform.ToMat4());
        renderer.Draw(*object.m_pModel);
    }
}
