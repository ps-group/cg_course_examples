#pragma once
#include "MeshP3NT2.h"
#include "StaticProgramContext.h"
#include <anax/Component.hpp>
#include <anax/System.hpp>

class CStaticMeshComponent : public anax::Component
{
public:
    std::unique_ptr<CMeshP3NT2> m_pMesh;
    CTexture2DUniquePtr m_pDiffuseMap;
    CTexture2DUniquePtr m_pSpecularMap;
    glm::mat4 m_worldTransform;
};

class CStaticRenderSystem
        : public anax::System<anax::Requires<CStaticMeshComponent>>
{
public:
    CStaticRenderSystem();

    void Render(const glm::mat4 &view, const glm::mat4 &projection);

private:
    CStaticProgramContext m_context;
};
