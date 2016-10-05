#include "stdafx.h"
#include "TwistValueController.h"

namespace
{
const float MIN_TWIST = -2.f;
const float MAX_TWIST = 2.f;
const float NEXT_TWIST_STEP = 0.2f;
const float TWIST_CHANGE_SPEED = 1.f;
}

// При каждом вызове Update величина twist "догоняет" назначенное значение.
void CTwistValueController::Update(float deltaSeconds)
{
    const float twistDiff = fabsf(m_nextTwistValue - m_currentTwistValue);
    if (twistDiff > std::numeric_limits<float>::epsilon())
    {
        const float sign = (m_nextTwistValue > m_currentTwistValue) ? 1.f : -1.f;
        const float growth = deltaSeconds * TWIST_CHANGE_SPEED;
        if (growth > twistDiff)
        {
            m_currentTwistValue = m_nextTwistValue;
        }
        else
        {
            m_currentTwistValue += sign * growth;
        }
    }
}

bool CTwistValueController::OnKeyDown(const SDL_KeyboardEvent &event)
{
    switch (event.keysym.sym)
    {
    case SDLK_EQUALS:
    case SDLK_PLUS:
        m_nextTwistValue = std::min(m_nextTwistValue + NEXT_TWIST_STEP, MAX_TWIST);
        return true;
    case SDLK_MINUS:
        m_nextTwistValue = std::max(m_nextTwistValue - NEXT_TWIST_STEP, MIN_TWIST);
        return true;
    default:
        return false;
    }
}

float CTwistValueController::GetCurrentValue() const
{
    return m_currentTwistValue;
}
