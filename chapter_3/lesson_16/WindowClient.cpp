#include "stdafx.h"
#include "WindowClient.h"

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};
const float CAMERA_INITIAL_ROTATION = 0.1f;
const float CAMERA_INITIAL_DISTANCE = 3;
const int SPHERE_PRECISION = 40;

void SetupOpenGLState()
{
    // включаем механизмы трёхмерного мира.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // включаем систему освещения
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

template <class T>
void DoWithTransform(const glm::mat4 &mat, T && callback)
{
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(mat));
    callback();
    glPopMatrix();
}
}

CWindowClient::CWindowClient(CWindow &window)
    : CAbstractWindowClient(window)
    , m_sphereObj(SPHERE_PRECISION, SPHERE_PRECISION)
    , m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
    , m_sunlight(GL_LIGHT0)
{
    const glm::vec3 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f};
    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};

    window.SetBackgroundColor(BLACK);
    CheckOpenGLVersion();
    SetupOpenGLState();

    m_sphereMat.SetDiffuse(WHITE_RGBA);
    m_sphereMat.SetAmbient(WHITE_RGBA);
    m_sphereMat.SetSpecular(0.7f * WHITE_RGBA);
    m_sphereMat.SetShininess(30);

    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_RGBA);
    m_sunlight.SetAmbient(0.4f * WHITE_RGBA);
    m_sunlight.SetSpecular(WHITE_RGBA);
}

void CWindowClient::OnUpdateWindow(float deltaSeconds)
{
    UpdateRotation(deltaSeconds);
    m_camera.Update(deltaSeconds);
    SetupView(GetWindow().GetWindowSize());

    m_sphereMat.Setup();
    m_sunlight.Setup();
    m_programContext.Use();

    DoWithTransform(m_earthTransform, [&] {
        m_sphereObj.Draw();
    });
}

void CWindowClient::OnKeyDown(const SDL_KeyboardEvent &event)
{
    if (m_camera.OnKeyDown(event))
    {
        return;
    }
}

void CWindowClient::OnKeyUp(const SDL_KeyboardEvent &event)
{
    if (m_camera.OnKeyUp(event))
    {
        return;
    }
}

void CWindowClient::CheckOpenGLVersion()
{
    // В OpenGL 2.0 шейдерные программы вошли в спецификацию API.
    // Ещё в OpenGL 1.2 мультитекстурирование также вошло в спецификацию,
    // см. http://opengl.org/registry/specs/ARB/multitexture.txt
    if (!GLEW_VERSION_2_0)
    {
        throw std::runtime_error("Sorry, but OpenGL 3.2 is not available");
    }
}

void CWindowClient::UpdateRotation(float deltaSeconds)
{
    const float ROTATION_SPEED = 0.2f;
    const float deltaRotation = ROTATION_SPEED * deltaSeconds;
    m_earthTransform = glm::rotate(m_earthTransform, deltaRotation,
                                   glm::vec3(0, 1, 0));
}

void CWindowClient::SetupView(const glm::ivec2 &size)
{
    glViewport(0, 0, size.x, size.y);
    const glm::mat4 mv = m_camera.GetViewTransform();
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
