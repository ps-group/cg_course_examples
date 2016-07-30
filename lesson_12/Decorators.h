#pragma once
#include "IBody.h"
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

class CAbstractDecorator : public IBody
{
public:
    void SetChild(IBodyUniquePtr && pChild);

protected:
    void UpdateChild(float deltaTime);
    void DrawChild()const;

private:
    IBodyUniquePtr m_pChild;
};

class CTransformDecorator : public CAbstractDecorator
{
public:
    void Update(float deltaTime);
    void Draw()const;

    void SetTransform(const glm::mat4 &transform);

private:
    glm::mat4 m_transform;
};

class CAnimatedDecorator : public CAbstractDecorator
{
public:
    void Update(float deltaTime);
    void Draw()const;

private:
    float m_animationPhase = 0;
};
