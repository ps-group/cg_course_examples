#pragma once
#include "libchapter2.h"

class CAbstractDecorator : public ISceneObject
{
public:
    void SetChild(ISceneObjectUniquePtr && pChild);

protected:
    void UpdateChild(float deltaTime);
    void DrawChild()const;

private:
    ISceneObjectUniquePtr m_pChild;
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

    void SetPeriod(float value);

private:
    float m_animationPhase = 0;
    float m_period = 1;
};
