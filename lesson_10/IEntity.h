#pragma once
#include <memory>

class IEntity
{
public:
    virtual ~IEntity() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw()const = 0;
};

using IBodyUniquePtr = std::unique_ptr<IEntity>;
