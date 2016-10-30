#pragma once

#include "Window.h"
#include <SDL2/SDL_events.h>

// Обрабатывает обновление состояния и рисование кадра
// Также обрабатывает события SDL, разделённые по категориям.
class IWindowClient
{
public:
    virtual ~IWindowClient() = default;

    virtual void OnUpdateWindow(float deltaSeconds) = 0;
    virtual void OnDrawWindow() = 0;

    virtual void OnDragBegin(const glm::vec2 &pos) { (void)pos; }
    virtual void OnDragMotion(const glm::vec2 &pos) { (void)pos; }
    virtual void OnDragEnd(const glm::vec2 &pos) { (void)pos; }
    virtual void OnKeyDown(const SDL_KeyboardEvent &) {}
    virtual void OnKeyUp(const SDL_KeyboardEvent &) {}
};

class CWindow;

class CAbstractWindowClient : IWindowClient
{
public:
    CAbstractWindowClient(CWindow &window);
    ~CAbstractWindowClient();

protected:
    CWindow &GetWindow();
    const CWindow &GetWindow()const;

private:
    CWindow &m_window;
};
