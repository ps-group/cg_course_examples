#include "stdafx.h"
#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};
const glm::vec3 SUNLIGHT_DIRECTION = {1.f, -1.f, 0.f};
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

glm::vec3 TransformPoint(const glm::vec3 point, const glm::mat4 &transform)
{
    const glm::vec4 original(point, 1);
    glm::vec4 transformed = transform * original;
    transformed /= transformed.w;

    return glm::vec3(transformed);
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

    auto getWindowSize = std::bind(&CWindow::GetWindowSize, this);

    m_pField = std::make_unique<CMemoryField>();
    m_pHud = std::make_unique<CHeadUpDisplay>(getWindowSize);
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
    m_pField->Update(deltaSeconds);
    m_pHud->Update(deltaSeconds);
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    SetupView(size);

    m_sunlight.Setup();
    m_pField->Draw();
    m_pHud->Draw();
}

void CWindow::SetupView(const glm::ivec2 &size)
{
    glViewport(0, 0, size.x, size.y);

    // Матрица вида возвращается камерой и составляет
    // начальное значение матрицы GL_MODELVIEW.
    glLoadMatrixf(glm::value_ptr(m_camera.GetViewTransform()));

    // Матрица перспективного преобразования
    // будет значением матрица GL_PROJECTION
    const glm::mat4 proj = GetProjectionMatrix(size);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(proj));
    glMatrixMode(GL_MODELVIEW);
}

glm::mat4 CWindow::GetProjectionMatrix(const glm::ivec2 &size)
{
    // Матрица перспективного преобразования вычисляется функцией
    // glm::perspective, принимающей угол обзора, соотношение ширины
    // и высоты окна, расстояния до ближней и дальней плоскостей отсечения.
    const float fieldOfView = glm::radians(70.f);
    const float aspect = float(size.x) / float(size.y);
    const float zNear = 0.01f;
    const float zFar = 100.f;

    return glm::perspective(fieldOfView, aspect, zNear, zFar);
}

void CWindow::OnKeyDown(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyDown(event);
}

void CWindow::OnKeyUp(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyUp(event);
}

void CWindow::OnDragEnd(const glm::vec2 &pos)
{
    // Вычисляем позицию точки в нормализованных координатах окна,
    //  то есть на диапазоне [-1; 1].
    // Также переворачиваем координату "y",
    //  т.к. OpenGL считает нулевым нижний левый угол окна,
    //  а все оконные системы - верхний левый угол.
    const glm::ivec2 winSize = GetWindowSize();
    const glm::vec2 halfWinSize = 0.5f * glm::vec2(winSize);
    const glm::vec2 invertedPos(pos.x, winSize.y - pos.y);
    const glm::vec2 normalizedPos = (invertedPos - halfWinSize) / halfWinSize;

    // Вычисляем матрицу обратного преобразования
    //  поскольку поле игры не имеет своей трансформации,
    //  мы берём матрицу камеры в качестве ModelView-матрицы
    const glm::mat4 mvMat = m_camera.GetViewTransform();
    const glm::mat4 projMat = GetProjectionMatrix(winSize);
    const glm::mat4 inverse = glm::inverse(projMat * mvMat);

    // В нормализованном пространстве глубина изменяется от -1 до +1.
    // Вычисляем начало и конец отрезка, проходящего через
    //  нормализованное пространство насквозь.
    const glm::vec3 start = TransformPoint(glm::vec3(normalizedPos, -1.f), inverse);
    const glm::vec3 end =   TransformPoint(glm::vec3(normalizedPos, +1.f), inverse);

    m_pField->Activate(CRay(start, end - start));
}
