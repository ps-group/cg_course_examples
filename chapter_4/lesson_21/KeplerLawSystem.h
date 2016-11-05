#pragma once
#include "Components.h"
#include <anax/System.hpp>

class ITimeController;

class CKeplerLawSystem
        : public anax::System<anax::Requires<
            CSpaceBodyComponent,
            CTransformComponent,
            CEllipticOrbitComponent
        >>
{
public:
    CKeplerLawSystem(ITimeController &controller);

    void Update();

private:
    ITimeController &m_timeController;
};
