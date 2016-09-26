#pragma once
#include <memory>

class ISceneObject
{
public:
    virtual ~ISceneObject() = default;

    virtual void Update(float dt) = 0;
    virtual void Draw()const = 0;
};

using ISceneObjectUniquePtr = std::unique_ptr<ISceneObject>;
