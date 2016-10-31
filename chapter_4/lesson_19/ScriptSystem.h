#pragma once

#include "Components.h"
#include <anax/System.hpp>

class CScriptSystem
        : public anax::System<anax::Requires<CScriptComponent>>
{
public:
    CScriptSystem();

    void Update(float deltaSeconds);
};
