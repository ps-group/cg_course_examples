#include "stdafx.h"
#include "Decorators.h"

namespace
{
const float ANIMATION_STEP_SECONDS = 2.f;

/// @param phase - Фаза анимации на отрезке [0..1]
glm::mat4 GetRotateZTransfrom(float phase)
{
    // угол вращения вокруг оси Z лежит в отрезке [0...2*pi].
    const float angle = float(2 * M_PI) * phase;

    return glm::rotate(glm::mat4(), angle, {0, 0, 1});
}
}

void CAbstractDecorator::SetChild(IBodyUniquePtr &&pChild)
{
    m_pChild = std::move(pChild);
}

void CAbstractDecorator::UpdateChild(float deltaTime)
{
    assert(m_pChild.get());
    m_pChild->Update(deltaTime);
}

void CAbstractDecorator::DrawChild() const
{
    assert(m_pChild.get());
    m_pChild->Draw();
}

void CAnimatedDecorator::Update(float deltaTime)
{
    m_animationPhase += (deltaTime / ANIMATION_STEP_SECONDS);
    m_animationPhase = fmodf(m_animationPhase, 1.f);
}

void CAnimatedDecorator::Draw() const
{
    const glm::mat4 matrix = GetRotateZTransfrom(m_animationPhase);
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(matrix));
    DrawChild();
    glPopMatrix();
}

void CTransformDecorator::Update(float deltaTime)
{
    UpdateChild(deltaTime);
}

void CTransformDecorator::Draw() const
{
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(m_transform));
    DrawChild();
    glPopMatrix();
}

void CTransformDecorator::SetTransform(const glm::mat4 &transform)
{
    m_transform = transform;
}
