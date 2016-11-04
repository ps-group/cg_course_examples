#include "stdafx.h"
#include "RenderSystem.h"
#include "PlanetRenderer.h"
#include "includes/opengl-common.hpp"

namespace
{
glm::mat4 GetEnvironmentViewMat4(const glm::mat4 &view)
{
    // Обнуляем перемещение в матрице афинного преобразования,
    //  чтобы нарисовать объект, являющийся частью окружения сцены.
    glm::mat4 result = view;
    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;

    return result;
}
}

CRenderSystem::CRenderSystem()
{
}

void CRenderSystem::SetupLight0(const glm::vec4 &position, const glm::vec4 &diffuse, const glm::vec4 &specular)
{
    m_light0.m_position = position;
    m_light0.m_diffuse = diffuse;
    m_light0.m_specular = specular;
}

void CRenderSystem::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    CPlanetRenderer3D renderer(m_planetProgram);
    renderer.SetProjectionMat4(projection);
    renderer.SetupLight0(m_light0.m_position, m_light0.m_diffuse, m_light0.m_specular);

    // Отключаем запись в буфер глубины для рисования объектов окружения,
    //  также устанавливаем модифицированную матрицу преобразования
    //  в координаты зрителя.
    glDepthMask(GL_FALSE);
    renderer.SetViewMat4(GetEnvironmentViewMat4(view));
    DoRenderPass(CMeshComponent::Environment, renderer);

    // Включаем обратно запись в буфер глубины для рисования объектов сцены.
    glDepthMask(GL_TRUE);
    renderer.SetViewMat4(view);
    DoRenderPass(CMeshComponent::Foreground, renderer);
}

void CRenderSystem::DoRenderPass(CMeshComponent::Category category, CPlanetRenderer3D &renderer)
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
        renderer.Draw(mesh);
    }
}
