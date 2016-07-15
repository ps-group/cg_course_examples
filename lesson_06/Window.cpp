#include "stdafx.h"
#include "Window.h"

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};
}

CWindow::CWindow()
{
    SetBackgroundColor(BLACK);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_system.Update(deltaSeconds);

    // обновляем заголовок окна
    char timeString[20];
    snprintf(timeString, 20, "%.2f", double(m_system.GetTimeSpeed()));
    SetTitle("Year " + std::to_string(m_system.GetYear())
             + " (" + std::string(timeString) + " years/second)");
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    m_windowSize = size;
    SetupView();
    m_system.Draw();
}

void CWindow::OnKeyDown(const SDL_KeyboardEvent &event)
{
    switch (event.keysym.sym)
    {
    case SDLK_LEFT:
        m_system.SlowdownTime();
        break;
    case SDLK_RIGHT:
        m_system.SpeedupTime();
        break;
    case SDLK_EQUALS:
        m_system.ZoomIn();
        SetupView();
        break;
    case SDLK_MINUS:
        m_system.ZoomOut();
        SetupView();
        break;
    }
}

void CWindow::SetupView()
{
    const float viewScale = m_system.GetViewScale();
    // Матрица ортографического проецирования изображения в трёхмерном пространстве
    // из параллелипипеда с размером, равным (size.X x size.Y x 2).
    const float halfWidth = float(m_windowSize.x) * 0.5f / viewScale;
    const float halfHeight = float(m_windowSize.y) * 0.5f / viewScale;
    const glm::mat4 matrix = glm::ortho<float>(-halfWidth, halfWidth, -halfHeight, halfHeight);
    glViewport(0, 0, m_windowSize.x, m_windowSize.y);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));
    glMatrixMode(GL_MODELVIEW);
}
