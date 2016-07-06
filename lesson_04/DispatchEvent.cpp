#include "stdafx.h"
#include "DispatchEvent.h"

namespace
{
glm::vec2 GetMousePosition(const SDL_MouseButtonEvent &event)
{
    return { event.x, event.y };
}

glm::vec2 GetMousePosition(const SDL_MouseMotionEvent &event)
{
    return { event.x, event.y };
}
}

void sdl::DispatchEvent(const SDL_Event &event, IInputEventAcceptor &acceptor)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        acceptor.OnKeyDown(event.key);
        break;
    case SDL_KEYUP:
        acceptor.OnKeyUp(event.key);
        break;
    case SDL_MOUSEBUTTONDOWN:
        acceptor.OnDragBegin(GetMousePosition(event.button));
        break;
    case SDL_MOUSEBUTTONUP:
        acceptor.OnDragEnd(GetMousePosition(event.button));
        break;
    case SDL_MOUSEMOTION:
        acceptor.OnDragMotion(GetMousePosition(event.motion));
        break;
    }
}
