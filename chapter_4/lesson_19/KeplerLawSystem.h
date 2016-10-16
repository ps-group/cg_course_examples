#pragma once
#include "Components.h"
#include <anax/System.hpp>

class CKeplerLawSystem
        : public anax::System<anax::Requires<
            CSpaceBodyComponent,
            CTransformComponent,
            CEllipticOrbitComponent
        >>
{
public:
    void Update(float deltaSeconds);

private:
    static const double DEFAULT_TIME_SPEED;

    double m_time = 0;
    double m_timeSpeed = DEFAULT_TIME_SPEED;
};
