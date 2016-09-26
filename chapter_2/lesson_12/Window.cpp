#include "stdafx.h"
#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};
const glm::vec3 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f};
const float CAMERA_INITIAL_ROTATION = float(M_PI);
const float CAMERA_INITIAL_DISTANCE = 4.5f;

void SetupOpenGLState()
{
    // включаем механизмы трёхмерного мира.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // включаем систему освещения
    glEnable(GL_LIGHTING);

    // включаем текстурирование в старом стиле (OpenGL 1.1)
    glEnable(GL_TEXTURE_2D);
}
}

CWindow::CWindow()
    : m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
    , m_sunlight(GL_LIGHT0)
{
    SetBackgroundColor(BLACK);

    m_camera.SetDirection({0.f, 1.f, 0.7f});

    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_RGBA);
    m_sunlight.SetAmbient(0.1f * WHITE_RGBA);
    m_sunlight.SetSpecular(WHITE_RGBA);
}

void CWindow::OnWindowInit(const glm::ivec2 &size)
{
    (void)size;
    SetupOpenGLState();

    m_pField = std::make_unique<CMemoryField>();
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
    m_pField->Update(deltaSeconds);
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    SetupView(size);

    m_sunlight.Setup();
    m_pField->Draw();
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
