#pragma once

#include <glm/vec3.hpp>
#include <boost/noncopyable.hpp>

class IRenderStates
{
public:
    virtual ~IRenderStates() = default;
};

class IDrawable
{
public:
    virtual ~IDrawable() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
};

class CIdentityCube : public IDrawable
{
public:
    void Update(float deltaTime) final;
    void Draw() final;
};
