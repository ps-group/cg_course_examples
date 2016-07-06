#include "stdafx.h"
#include "Window.h"

namespace
{
const glm::vec4 QUIET_GREEN = {0.f, 0.5f, 0.2f, 1.f};
}

CWindow::CWindow()
{
    auto pEmitter = std::make_unique<CParticleEmitter>();
    pEmitter->SetPosition({0, 600});
    pEmitter->SetAngleRange(0.7f * float(M_PI), 0.9f * float(M_PI));
    pEmitter->SetEmitIntervalRange(0.04f, 0.12f);
    pEmitter->SetLifetimeRange(10.f, 20.f);
    pEmitter->SetPetalsCountRangle(5, 9);
    pEmitter->SetRadiusRange(40.f, 75.f);
    pEmitter->SetSpeedRange(8.f, 15.f);
    m_system.SetEmitter(std::move(pEmitter));

    SetBackgroundColor(QUIET_GREEN);
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_system.Advance(deltaSeconds);
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    SetupView(size);
    m_system.Draw();
}

void CWindow::SetupView(const glm::ivec2 &size)
{
    // Матрица ортографического проецирования изображения в трёхмерном пространстве
    // из параллелипипеда с размером, равным (size.X x size.Y x 2).
    const glm::mat4 matrix = glm::ortho<float>(0, size.x, size.y, 0);
    glViewport(0, 0, size.x, size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));
    glMatrixMode(GL_MODELVIEW);
}
