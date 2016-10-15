#include "stdafx.h"
#include "WindowClient.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

namespace
{
const float CAMERA_INITIAL_ROTATION = -1.8f;
const float CAMERA_INITIAL_DISTANCE = 6.3f;
const int SPHERE_PRECISION = 40;

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

    InstantiateEntities();
    SetupSystems();
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

void CWindowClient::InstantiateEntities()
{
    CTexture2DLoader loader;

    // Добавление сущности "Земля".
    {
        mat4 transform = glm::translate(mat4(), { 1.f, 0.f, 3.f });
        anax::Entity earth = m_world.createEntity();
        auto &mesh = earth.addComponent<CStaticMeshComponent>();
        mesh.m_pMesh = CTesselator::TesselateSphere(SPHERE_PRECISION);
        mesh.m_worldTransform = transform;
        mesh.m_pDiffuseMap = loader.Load("res/solar_system/earth_diffuse.jpg");
        mesh.m_pSpecularMap = loader.Load("res/solar_system/earth_specular.jpg");

        // Каждую сущность в Anax следует активировать.
        earth.activate();
    }

    // Добавление сущности "Луна".
    {
        mat4 transform = glm::translate(mat4(), { -4.f, 0.f, -2.f });
        transform = glm::scale(transform, vec3(0.4f));
        anax::Entity moon = m_world.createEntity();
        auto &mesh = moon.addComponent<CStaticMeshComponent>();
        mesh.m_pMesh = CTesselator::TesselateSphere(SPHERE_PRECISION);
        mesh.m_worldTransform = transform;
        mesh.m_pDiffuseMap = loader.Load("res/solar_system/moon_diffuse.png");
        mesh.m_pSpecularMap = loader.Load("res/solar_system/moon_specular.png");

        // Каждую сущность в Anax следует активировать.
        moon.activate();
    }

}

void CWindowClient::SetupSystems()
{
    m_world.addSystem(m_renderSystem);

    // После активации новых сущностей или деактивации,
    //  а при добавления новых систем следует
    //  вызывать refresh() у мира.
    m_world.refresh();
}
