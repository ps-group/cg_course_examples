#pragma once
#include "Components.h"
#include <anax/System.hpp>

class CParticleUpdateSystem
        : public anax::System<anax::Requires<CParticleComponent>>
{
public:
    void Update(float deltaSec);
};
