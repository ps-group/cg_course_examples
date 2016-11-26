#pragma once
#include "Components.h"
#include "SkeletalAnimationProgram.h"
#include "PhongProgram.h"
#include <anax/System.hpp>

class CSkeletalModel3DRenderer;

class CRenderSystem
        : public anax::System<anax::Requires<CRenderableComponent, CTransformComponent>>
{
public:
    CRenderSystem();

    void SetupLight0(const glm::vec4 &position,
                     const glm::vec4 &diffuse,
                     const glm::vec4 &specular);
    void Render(const glm::mat4 &view, const glm::mat4 &projection);

private:
    void RenderEnvironment(const glm::mat4 &view, const glm::mat4 &projection);
    void RenderForeground(const glm::mat4 &view, const glm::mat4 &projection);

    CSkeletalAnimationProgram m_skeletalProgram;
    CPhongProgram m_phongProgram;
};
