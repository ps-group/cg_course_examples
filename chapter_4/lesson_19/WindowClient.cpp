#include "stdafx.h"
#include "WindowClient.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

namespace
{
const float CAMERA_INITIAL_ROTATION = -1.8f;
const float CAMERA_INITIAL_DISTANCE = 16.0f;
const int SPHERE_PRECISION = 40;
const glm::vec4 SUNLIGHT_POSITION = {0, 0, 0, 1};
const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
const glm::vec4 FADED_WHITE_RGBA = {0.3f, 0.3f, 0.3f, 1.0f};

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
    , m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
{
    const vec4 BLACK_RGBA = {0, 0, 0, 1};
    window.SetBackgroundColor(BLACK_RGBA);
    SetupOpenGLState();

    m_renderSystem.SetupLight0(SUNLIGHT_POSITION, WHITE_RGBA, FADED_WHITE_RGBA);

    CSceneLoader loader(m_world);
    loader.LoadScene("res/solar_system/solar_system_2012.json");

    // Добавляем систему, отвечающую за рендеринг планет.
    m_world.addSystem(m_renderSystem);

    // После активации новых сущностей или деактивации,
    //  а при добавления новых систем следует
    //  вызывать refresh() у мира.
    m_world.refresh();
//    InitScene();
}

void CWindowClient::OnUpdate(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
}

void CWindowClient::OnDraw()
{
    const glm::ivec2 windowSize = GetWindow().GetWindowSize();

    const mat4 view = m_camera.GetViewTransform();
    const mat4 proj = MakeProjectionMatrix(windowSize);

    glViewport(0, 0, windowSize.x, windowSize.y);
    m_renderSystem.Render(view, proj);
}

void CWindowClient::OnKeyDown(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyDown(event);
}

void CWindowClient::OnKeyUp(const SDL_KeyboardEvent &event)
{
    m_camera.OnKeyUp(event);
}

void CWindowClient::InitScene()
{
    CTexture2DLoader loader;
    std::shared_ptr<CMeshP3NT2> pSphere = CTesselator::TesselateSphere(SPHERE_PRECISION);

    // Добавление сущности "Земля".
    {
        anax::Entity earth = m_world.createEntity();
        auto &mesh = earth.addComponent<CStaticMeshComponent>();
        mesh.m_pMesh = pSphere;
        mesh.m_pDiffuseMap = loader.Load("res/solar_system/earth_diffuse.jpg");
        mesh.m_pSpecularMap = loader.Load("res/solar_system/earth_specular.jpg");
        auto &transform = earth.addComponent<CTransformComponent>();
        transform.m_position = { 10.f, 0.f, 30.f };

        // Каждую сущность в Anax следует активировать.
        earth.activate();
    }

    // Добавление сущности "Луна".
    {
        anax::Entity moon = m_world.createEntity();
        auto &mesh = moon.addComponent<CStaticMeshComponent>();
        mesh.m_pMesh = pSphere;
        mesh.m_pDiffuseMap = loader.Load("res/solar_system/moon_diffuse.png");
        auto &transform = moon.addComponent<CTransformComponent>();
        transform.m_position = { 12.f, 0.f, 32.f };
        transform.m_scale = vec3(0.4f);
        moon.activate();
    }

    // Добавление сущности "Марс".
    {
        anax::Entity mars = m_world.createEntity();
        auto &mesh = mars.addComponent<CStaticMeshComponent>();
        mesh.m_pMesh = pSphere;
        mesh.m_pDiffuseMap = loader.Load("res/solar_system/mars_diffuse.jpg");
        auto &transform = mars.addComponent<CTransformComponent>();
        transform.m_position = { 40.f, 0.f, 18.f };
        transform.m_scale = vec3(0.8f);
        mars.activate();
    }

    // Добавление сущности "Солнце".
    {
        anax::Entity sol = m_world.createEntity();
        auto &mesh = sol.addComponent<CStaticMeshComponent>();
        mesh.m_pMesh = pSphere;
        mesh.m_pEmissiveMap = loader.Load("res/solar_system/sol_emissive.jpg");
        auto &transform = sol.addComponent<CTransformComponent>();
        transform.m_position = vec3(SUNLIGHT_POSITION);
        transform.m_scale = vec3(3.0f);
        sol.activate();
    }

    // Добавляем систему, отвечающую за рендеринг планет.
    m_world.addSystem(m_renderSystem);

    // После активации новых сущностей или деактивации,
    //  а при добавления новых систем следует
    //  вызывать refresh() у мира.
    m_world.refresh();
}
