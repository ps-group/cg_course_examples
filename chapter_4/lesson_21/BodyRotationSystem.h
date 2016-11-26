#pragma once
#include "Components.h"
#include <anax/System.hpp>

class ITimeController;

class CBodyRotationSystem
        : public anax::System<anax::Requires<
            CSpaceBodyComponent,
            CTransformComponent
        >>
{
public:
    CBodyRotationSystem(ITimeController &controller);

    void Update();

private:
    ITimeController &m_timeContoller;
};
