#pragma once

#include <glm/fwd.hpp>
#include <SDL2/SDL_events.h>

// Обрабатывает события SDL, разделённые по категориям.
class IInputListener
{
public:
    virtual ~IInputListener() = default;

    virtual bool OnMousePress(const SDL_MouseButtonEvent &) { return false; }
    virtual bool OnMouseMotion(const SDL_MouseMotionEvent &) { return false; }
    virtual bool OnMouseUp(const SDL_MouseButtonEvent &) { return false; }
    virtual bool OnKeyDown(const SDL_KeyboardEvent &) { return false; }
    virtual bool OnKeyUp(const SDL_KeyboardEvent &) { return false; }
};
