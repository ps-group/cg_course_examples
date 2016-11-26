#pragma once
#include "Components.h"
#include <anax/System.hpp>

class CAnimationSystem
        : public anax::System<anax::Requires<CAnimateComponent>>
{
public:
    CAnimationSystem();

    void Update(float deltaSec);
};


