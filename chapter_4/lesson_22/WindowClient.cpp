#include "stdafx.h"
#include "WindowClient.h"
#include "includes/opengl-common.hpp"
#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec4;

namespace
{
const vec3 CAMERA_EYE = {0, 10, 20};
const vec3 CAMERA_AT = {0, 0, 0};
const vec3 CAMERA_UP = {0, 1, 0};
const vec4 SUNLIGHT_DIRECTION = {0, -1, 0, 0};
const vec4 WHITE_RGBA = {1, 1, 1, 1};
const vec4 FADED_WHITE_RGBA = {0.3f, 0.3f, 0.3f, 1.0f};
const char SCENE_JSON[] = "res/skeleton_animation/scene.json";
const char SKYBOX_PLIST[] = "res/skeleton_animation/skybox-stone-and-sky.plist";

void SetupOpenGLState()
{
    // включаем механизмы трёхмерного мира.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

glm::mat4 MakeProjectionMatrix(const glm::ivec2 &size)
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
}

CWindowClient::CWindowClient(CWindow &window)
    : CAbstractWindowClient(window)
    , m_defaultVAO(CArrayObject::do_bind_tag())
    , m_mouseGrabber(window)
    , m_camera(CAMERA_EYE, CAMERA_AT, CAMERA_UP)
{
    const vec4 BLACK_RGBA = {0, 0, 0, 1};
    const float CAM_SPEED = 20;

    window.SetBackgroundColor(BLACK_RGBA);
    SetupOpenGLState();

    m_camera.SetMoveSpeed(CAM_SPEED);
    m_renderSystem.SetupLight0(SUNLIGHT_DIRECTION, WHITE_RGBA, FADED_WHITE_RGBA);

    CSceneLoader loader(m_world);
    loader.LoadScene(SCENE_JSON);
    loader.LoadSkybox(SKYBOX_PLIST);

    // Добавляем систему, отвечающую за рендеринг моделей.
    m_world.addSystem(m_renderSystem);

    // Добавляем систему, отвечающую за проигрывание анимаций.
    m_world.addSystem(m_animationSystem);

    // После активации новых сущностей или деактивации,
    //  а при добавления новых систем следует
    //  вызывать refresh() у мира.
    m_world.refresh();
}

void CWindowClient::OnUpdate(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
    m_animationSystem.Update(deltaSeconds);
}

void CWindowClient::OnDraw()
{
    const glm::ivec2 windowSize = GetWindow().GetWindowSize();

    const mat4 view = m_camera.GetViewMat4();
    const mat4 proj = MakeProjectionMatrix(windowSize);

    glViewport(0, 0, windowSize.x, windowSize.y);
    m_renderSystem.Render(view, proj);
}

bool CWindowClient::OnKeyDown(const SDL_KeyboardEvent &event)
{
    return m_camera.OnKeyDown(event);
}

bool CWindowClient::OnKeyUp(const SDL_KeyboardEvent &event)
{
    return m_camera.OnKeyUp(event);
}

bool CWindowClient::OnMousePress(const SDL_MouseButtonEvent &event)
{
    return m_camera.OnMousePress(event);
}

bool CWindowClient::OnMouseMotion(const SDL_MouseMotionEvent &event)
{
    return m_mouseGrabber.OnMouseMotion(event)
            || m_camera.OnMouseMotion(event);
}

bool CWindowClient::OnMouseUp(const SDL_MouseButtonEvent &event)
{
    return m_camera.OnMouseUp(event);
}
