#include "stdafx.h"
#include "Window.h"
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace
{
const glm::vec4 QUIET_GREEN = {0, 0, 0, 1};
const glm::vec3 YELLOW = {1.f, 1.f, 0.f};
const glm::vec3 ORANGE = {1.f, 0.5f, 0.f};
const glm::vec3 PINK = {1.f, 0.3f, 0.3f};
const glm::vec4 WHITE_LIGHT = {1, 1, 1, 1};
const glm::vec3 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f};
const float CAMERA_INITIAL_ROTATION = 0;
const float CAMERA_INITIAL_DISTANCE = 5.f;

void SetupOpenGLState()
{
    // включаем механизмы трёхмерного мира.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // включаем систему освещения
    glEnable(GL_LIGHTING);

    // включаем применение цветов вершин как цвета материала.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}
}

CWindow::CWindow()
    : m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
    , m_sunlight(GL_LIGHT0)
{
    SetBackgroundColor(QUIET_GREEN);

    m_staticCube.SetFaceColor(CubeFace::Top, YELLOW);
    m_staticCube.SetFaceColor(CubeFace::Bottom, YELLOW);
    m_staticCube.SetFaceColor(CubeFace::Left, ORANGE);
    m_staticCube.SetFaceColor(CubeFace::Right, ORANGE);
    m_staticCube.SetFaceColor(CubeFace::Front, PINK);
    m_staticCube.SetFaceColor(CubeFace::Back, PINK);

    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_LIGHT);
    m_sunlight.SetAmbient(0.1f * WHITE_LIGHT);
}

void CWindow::OnWindowInit(const glm::ivec2 &size)
{
    (void)size;
    SetupOpenGLState();
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
    m_dynamicCube.Update(deltaSeconds);
    m_staticCube.Update(deltaSeconds);
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    SetupView(size);
    m_sunlight.Setup();

    // Смещаем анимированный единичный куб в другую сторону
    glPushMatrix();
    glTranslatef(0, -1.5f, 0);
    m_dynamicCube.Draw();
    glPopMatrix();

    // Смещаем статический единичный куб в другую сторону
    glPushMatrix();
    glTranslatef(0, 1.5f, 0);
    m_staticCube.Draw();
    glPopMatrix();
}

void CWindow::SetupView(const glm::ivec2 &size)
{
    glViewport(0, 0, size.x, size.y);

    // Матрица вида возвращается камерой и составляет
    // начальное значение матрицы GL_MODELVIEW.
    glLoadMatrixf(glm::value_ptr(m_camera.GetViewTransform()));

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

void CWindow::OnKeyDown(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyDown(event);
}

void CWindow::OnKeyUp(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyUp(event);
}
