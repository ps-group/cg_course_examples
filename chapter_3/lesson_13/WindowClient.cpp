#include "stdafx.h"
#include "WindowClient.h"

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};
const float CAMERA_INITIAL_ROTATION = 1;
const float CAMERA_INITIAL_DISTANCE = 5;
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
}
}

CWindowClient::CWindowClient(CWindow &window)
    : CAbstractWindowClient(window)
    , m_sphereObj(SPHERE_PRECISION, SPHERE_PRECISION)
    , m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
    , m_sunlight(GL_LIGHT0)
    , m_programFixed(CShaderProgram::fixed_pipeline_t())
{
    const glm::vec3 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f};
    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
    const glm::vec4 DARK_BLUE_RGBA = {0.2f, 0.2f, 0.6f, 1.f};
    const float AMBIENT_SCALE = 0.2f;

    window.SetBackgroundColor(BLACK);
    CheckOpenGLVersion();
    SetupOpenGLState();

    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_RGBA);
    m_sunlight.SetAmbient(0.1f * WHITE_RGBA);
    m_sunlight.SetSpecular(WHITE_RGBA);

    m_sphereMat.SetShininess(30);
    m_sphereMat.SetSpecular(WHITE_RGBA);
    m_sphereMat.SetDiffuse(DARK_BLUE_RGBA);
    m_sphereMat.SetAmbient(DARK_BLUE_RGBA * AMBIENT_SCALE);

    const std::string vertexShader = CFilesystemUtils::LoadFileAsString("res/lambert-phong.vert");
    const std::string lambertShader = CFilesystemUtils::LoadFileAsString("res/lambert.frag");
    const std::string phongShader = CFilesystemUtils::LoadFileAsString("res/phong.frag");

    m_programLambert.CompileShader(vertexShader, ShaderType::Vertex);
    m_programLambert.CompileShader(lambertShader, ShaderType::Fragment);
    m_programLambert.Link();

    m_programPhong.CompileShader(vertexShader, ShaderType::Vertex);
    m_programPhong.CompileShader(phongShader, ShaderType::Fragment);
    m_programPhong.Link();

    m_programQueue = { &m_programPhong, &m_programLambert, &m_programFixed };
}

void CWindowClient::OnUpdateWindow(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
}

void CWindowClient::OnDrawWindow()
{
    SetupView(GetWindow().GetWindowSize());

    m_sunlight.Setup();
    m_sphereMat.Setup();

    // Активной будет первая программа из очереди.
    m_programQueue.front()->Use();
    m_sphereObj.Draw();
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
    // Передвигаем очередь программ,
    // если была нажата и отпущена клавиша "Пробел"
    if (event.keysym.sym == SDLK_SPACE)
    {
        std::rotate(m_programQueue.begin(), m_programQueue.begin() + 1, m_programQueue.end());
    }
}

void CWindowClient::CheckOpenGLVersion()
{
    // В OpenGL 2.0 шейдерные программы вошли в спецификацию API.
    if (!GLEW_VERSION_2_0)
    {
        throw std::runtime_error("Sorry, but OpenGL 3.2 is not available");
    }
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
