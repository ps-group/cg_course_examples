#include "stdafx.h"
#include "WindowClient.h"
#include "includes/opengl-common.hpp"
#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec4;

namespace
{
const vec3 CAMERA_EYE = {0, -10, 20};
const vec3 CAMERA_AT = {0, 10, 0};
const vec3 CAMERA_UP = {0, 1, 0};
const vec4 WHITE_RGBA = {1, 1, 1, 1};
const vec4 FADED_WHITE_RGBA = {0.3f, 0.3f, 0.3f, 1.0f};
const char SCENE_JSON[] = "res/particle_system/scene.json";
const char PARTICLE_IMAGE[] = "res/particle_system/sparkle.png";

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

    // -------------------------------------------------------------
    // TODO: move to CSceneLoader
    CAssetLoader loader;
    auto pTexture = loader.LoadTexture(PARTICLE_IMAGE);

    auto pEmitter = std::make_unique<CParticleEmitter>();
    pEmitter->SetDirection(glm::vec3(0, 1, 0));
    pEmitter->SetEmitIntervalRange(0.001f, 0.002f);
    pEmitter->SetLifetimeRange(2.f, 3.f);
    pEmitter->SetMaxDeviationAngle(float(0.3 * M_PI));
    pEmitter->SetMaxDistance(1.f);
    pEmitter->SetPosition(glm::vec3(0, 0, 0));
    pEmitter->SetSpeedRange(8.f, 15.f);

    auto pSystem = std::make_shared<CParticleSystem>();
    pSystem->SetEmitter(std::move(pEmitter));
    pSystem->SetGravity(glm::vec3(0, -0.2f, 0));
    pSystem->SetParticleTexture(pTexture);

    auto entity = m_world.createEntity();
    auto &particleCom = entity.addComponent<CParticleComponent>();
    particleCom.m_pSystem = pSystem;
    auto &transformCom = entity.addComponent<CTransformComponent>();
    (void)transformCom;
    entity.activate();
    // -------------------------------------------------------------

#if 0
    CSceneLoader loader(m_world);
    loader.LoadScene(SCENE_JSON);
    loader.LoadSkybox(SKYBOX_PLIST);
#endif

    // Добавляем систему, отвечающую за рендеринг планет.
    m_world.addSystem(m_renderSystem);

    // Добавляем систему, отвечающую за обновление систем частиц
    m_world.addSystem(m_updateSystem);

    // После активации новых сущностей или деактивации,
    //  а при добавления новых систем следует
    //  вызывать refresh() у мира.
    m_world.refresh();
}

void CWindowClient::OnUpdate(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
    m_updateSystem.Update(deltaSeconds);
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
