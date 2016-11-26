#include "MouseGrabber.h"
#include "includes/sdl-common.hpp"
#include "Window.h"
#include <boost/range/algorithm/find.hpp>

CMouseGrabber::CMouseGrabber(CWindow &window)
    : m_windowRef(window)
{
    // Включаем режим спрятанного курсора.
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

bool CMouseGrabber::OnMouseMotion(const SDL_MouseMotionEvent &event)
{
    const glm::ivec2 delta = { event.xrel, event.yrel };
    bool filtered = false;

    // Проверяем, является ли событие автосгенерированным
    //  из-за программного перемещения мыши.
    auto it = boost::find(m_blacklistedMoves, delta);
    if (it != m_blacklistedMoves.end())
    {
        m_blacklistedMoves.erase(it);
        filtered = true;
    }
    else
    {
        WarpMouseSafely();
    }

    return filtered;
}

void CMouseGrabber::WarpMouseSafely()
{
    const glm::ivec2 windowCenter = m_windowRef.GetWindowSize() / 2;
    m_windowRef.WarpMouse(windowCenter);

    glm::ivec2 mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    const glm::ivec2 reflectedPos = windowCenter - mousePos;
    m_blacklistedMoves.push_back(reflectedPos);
}
