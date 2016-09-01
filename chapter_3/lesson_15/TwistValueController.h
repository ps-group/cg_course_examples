#pragma once
#include <SDL2/SDL_events.h>

class CTwistValueController
{
public:
    void Update(float deltaSeconds);
    bool OnKeyDown(const SDL_KeyboardEvent &event);

    float GetCurrentValue()const;

private:
    float m_currentTwistValue = 0;
    float m_nextTwistValue = 0;
};
