#include "stdafx.h"
#include "Window.h"
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};

void SetupOpenGLState()
{
    // включаем механизмы трёхмерного мира.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}
}

CWindow::CWindow()
{
    SetBackgroundColor(BLACK);
}

void CWindow::OnWindowInit(const glm::ivec2 &size)
{
    (void)size;
    SetupOpenGLState();
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_cube.Update(deltaSeconds);
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    SetupView(size);
    m_cube.Draw();
}

void CWindow::SetupView(const glm::ivec2 &size)
{
    glViewport(0, 0, size.x, size.y);

    const glm::vec3 eye = {4, -4, 2};
    const glm::vec3 center = {0, 0, 0};
    const glm::vec3 up = {0, 0, 1};
    // Матрица моделирования-вида вычисляется функцией glm::lookAt.
    // Она даёт матрицу, действующую так, как будто камера смотрит
    // с позиции eye на точку center, а направление "вверх" камеры равно up.
    const glm::mat4 mv = glm::lookAt(eye, center, up);
    glLoadMatrixf(glm::value_ptr(mv));

    // Матрица перспективного преобразования вычисляется функцией
    // glm::perspective, принимающей угол обзора, соотношение ширины
    // и высоты окна, расстояния до ближней и дальней плоскостей отсечения.
    const float fieldOfView = glm::radians(70.f);
    const float aspect = float(size.x) / float(size.y);
    const float zNear = 0.01f;
    const float zFar = 100.f;
    const glm::mat4 proj = glm::perspective(fieldOfView, aspect, zNear, zFar);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(proj));
    glMatrixMode(GL_MODELVIEW);
}
