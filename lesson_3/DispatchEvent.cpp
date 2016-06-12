#include "DispatchEvent.h"
#include <glm/vec2.hpp>

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
        acceptor.OnMouseDown(event.button);
        break;
    case SDL_MOUSEBUTTONUP:
        acceptor.OnMouseUp(event.button);
        break;
    case SDL_MOUSEMOTION:
        acceptor.OnMouseMotion(event.motion);
        break;
    case SDL_MOUSEWHEEL:
        acceptor.OnMouseWheel(event.wheel);
        break;
    }
}
