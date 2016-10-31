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
    m_planetProgram.SetProjection(projection);
    CPlanetRenderer3D renderer(m_planetProgram);

    // Отключаем запись в буфер глубины для рисования объектов окружения.
    glDepthMask(GL_FALSE);
    RenderImpl(BackgroundObjects, renderer, view);
    glDepthMask(GL_TRUE);
    RenderImpl(ForegroundObjects, renderer, view);
}

CRenderSystem::DrawPass CRenderSystem::SelectDrawPass(const anax::Entity &entity)
{
    const auto &mesh = entity.getComponent<CStaticMeshComponent>();
    if (mesh.m_writesDepth)
    {
        return ForegroundObjects;
    }
    return BackgroundObjects;
}

void CRenderSystem::RenderImpl(DrawPass pass, CPlanetRenderer3D &renderer, const glm::mat4 &view)
{
    for (const auto &entity : getEntities())
    {
        // Пропускаем объекты, не попадающие в этот проход рисования.
        if (pass != SelectDrawPass(entity))
        {
            continue;
        }

        const auto &mesh = entity.getComponent<CStaticMeshComponent>();
        const auto &transform = entity.getComponent<CTransformComponent>();
        ApplyMaterial(mesh);
        ApplyTransform(transform, view);
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

void CRenderSystem::ApplyMaterial(const CStaticMeshComponent &mesh)
{
    m_planetProgram.BindDiffuseMap(GetTextureOrBlack(mesh.m_pDiffuseMap));
    m_planetProgram.BindSpecularMap(GetTextureOrBlack(mesh.m_pSpecularMap));
    m_planetProgram.BindEmissiveMap(GetTextureOrBlack(mesh.m_pEmissiveMap));
}

void CRenderSystem::ApplyTransform(const CTransformComponent &transform, const glm::mat4 &view)
{
    glm::mat4 localView = view;
    if (transform.m_drawAroundCamera)
    {
        // Обнуляем перемещение в матрице афинного преобразования,
        //  чтобы нарисовать объект, находящийся вокруг зрителя.
        localView[3][0] = 0;
        localView[3][1] = 0;
        localView[3][2] = 0;
    }
    m_planetProgram.SetView(localView);
    m_planetProgram.SetModel(transform.ToMat4());
    m_planetProgram.UpdateModelViewProjection();
}
