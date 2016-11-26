#include "stdafx.h"
#include "TimeController.h"

const double CTimeController::DEFAULT_TIME_SPEED = 0.02;

double CTimeController::GetSpaceTime() const
{
    return m_spaceTime;
}

void CTimeController::Update(float deltaSeconds)
{
    m_spaceTime += double(deltaSeconds) * m_timeSpeed;
}
