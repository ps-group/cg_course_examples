#pragma once

#include <memory>
#include <glm/fwd.hpp>
#include <SDL2/SDL_events.h>

class CAbstractWindow
{
public:
    CAbstractWindow();
    virtual ~CAbstractWindow();

	CAbstractWindow(const CAbstractWindow&) = delete;
	CAbstractWindow& operator=(const CAbstractWindow&) = delete;

    void Show(const std::string &title, glm::ivec2 const& size);
    void DoGameLoop();

protected:
    void SetBackgroundColor(glm::vec4 const& color);
    glm::ivec2 GetWindowSize()const;

    virtual void OnWindowInit(glm::ivec2 const& size) = 0;
    virtual void OnWindowEvent(const SDL_Event &event) = 0;
    virtual void OnUpdateWindow(float deltaSeconds) = 0;
    virtual void OnDrawWindow(glm::ivec2 const& size) = 0;

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};
