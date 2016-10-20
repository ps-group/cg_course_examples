#pragma once

#include <glm/fwd.hpp>
#include <SDL2/SDL_events.h>

// Обрабатывает события SDL, разделённые по категориям.
class IInputListener
{
public:
    virtual ~IInputListener() = default;

    virtual bool OnMousePress(const glm::vec2 &pos) { (void)pos; return false; }
    virtual bool OnMouseMotion(const glm::vec2 &pos) { (void)pos; return false; }
    virtual bool OnMouseUp(const glm::vec2 &pos) { (void)pos; return false; }
    virtual bool OnKeyDown(const SDL_KeyboardEvent &) { return false; }
    virtual bool OnKeyUp(const SDL_KeyboardEvent &) { return false; }
};
