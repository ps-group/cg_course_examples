#include "stdafx.h"
#include "WindowClient.h"
#include "EarthRenderer3D.h"

using glm::mat4;
using glm::vec3;

namespace
{
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
}
}

CWindowClient::CWindowClient(CWindow &window)
    : CAbstractWindowClient(window)
    , m_defaultVAO(CArrayObject::do_bind_tag())
    , m_sphereObj(SPHERE_PRECISION, SPHERE_PRECISION)
    , m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
    , m_sunlight(GL_LIGHT0)
{
    const glm::vec3 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f};
    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
    const glm::vec4 BLACK_RGBA = {0, 0, 0, 1};

    window.SetBackgroundColor(BLACK_RGBA);
    SetupOpenGLState();

    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_RGBA);
    m_sunlight.SetSpecular(WHITE_RGBA);
}

void CWindowClient::OnUpdateWindow(float deltaSeconds)
{
    UpdateRotation(deltaSeconds);
    m_camera.Update(deltaSeconds);
}

void CWindowClient::OnDrawWindow()
{
    SetupView(GetWindow().GetWindowSize());
    SetupLight0();

    CEarthRenderer3D renderer(m_programContext);
    m_sphereObj.Draw(renderer);
}

void CWindowClient::OnKeyDown(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyDown(event);
}

void CWindowClient::OnKeyUp(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyUp(event);
}

void CWindowClient::UpdateRotation(float deltaSeconds)
{
    const float ROTATION_SPEED = 0.2f;
    const float deltaRotation = ROTATION_SPEED * deltaSeconds;
    const mat4 model = glm::rotate(m_programContext.GetModel(),
                                   deltaRotation,
                                   vec3(0, 1, 0));
    m_programContext.SetModel(model);
}

void CWindowClient::SetupView(const glm::ivec2 &size)
{
    const mat4 view = m_camera.GetViewTransform();

    // Матрица перспективного преобразования вычисляется функцией
    // glm::perspective, принимающей угол обзора, соотношение ширины
    // и высоты окна, расстояния до ближней и дальней плоскостей отсечения.
    const float fieldOfView = glm::radians(70.f);
    const float aspect = float(size.x) / float(size.y);
    const float zNear = 0.01f;
    const float zFar = 100.f;
    const mat4 proj = glm::perspective(fieldOfView, aspect, zNear, zFar);

    glViewport(0, 0, size.x, size.y);

    m_programContext.SetView(view);
    m_programContext.SetProjection(proj);
}

void CWindowClient::SetupLight0()
{
    CEarthProgramContext::SLightSource light0;
    light0.specular = m_sunlight.GetSpecular();
    light0.diffuse = m_sunlight.GetDiffuse();
    light0.position = m_sunlight.GetUniformPosition();
    m_programContext.SetLight0(light0);
}
