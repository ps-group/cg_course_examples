#include "stdafx.h"
#include "WindowClient.h"

namespace
{
const glm::vec4 BLACK = {0, 0, 0, 1};
const float CAMERA_INITIAL_ROTATION = float(M_PI);
const float CAMERA_INITIAL_DISTANCE = 10;
const int SPHERE_PRECISION = 40;

void SetupOpenGLState()
{
    // включаем механизмы трёхмерного мира.
    glEnable(GL_DEPTH_TEST);

    // включаем систему освещения с режимом двустороннего освещения
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}
}

CWindowClient::CWindowClient(CWindow &window)
    : CAbstractWindowClient(window)
    , m_umbrellaObj(SPHERE_PRECISION, SPHERE_PRECISION)
    , m_camera(CAMERA_INITIAL_ROTATION, CAMERA_INITIAL_DISTANCE)
    , m_sunlight(GL_LIGHT0)
    , m_lamp(GL_LIGHT1)
    , m_programFixed(CShaderProgram::fixed_pipeline_t())
{
    const glm::vec3 SUNLIGHT_DIRECTION = {-1.f, 0.2f, 0.7f};
    const glm::vec3 LAMP_POSITION = {10.f, 5.0f, 1.9f};
    const glm::vec4 WHITE_RGBA = {1, 1, 1, 1};
    const glm::vec4 DARK_BLUE_RGBA = {0.2f, 0.2f, 0.6f, 1.f};
    const float AMBIENT_SCALE = 0.2f;

    window.SetBackgroundColor(BLACK);
    CheckOpenGLVersion();
    SetupOpenGLState();

    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_RGBA);
    m_sunlight.SetAmbient(WHITE_RGBA);
    m_sunlight.SetSpecular(WHITE_RGBA);

    m_lamp.SetPosition(LAMP_POSITION);
    m_lamp.SetDiffuse(WHITE_RGBA);
    m_lamp.SetAmbient(WHITE_RGBA);
    m_lamp.SetSpecular(WHITE_RGBA);

    m_umbrellaMat.SetDiffuse(DARK_BLUE_RGBA);
    m_umbrellaMat.SetAmbient(DARK_BLUE_RGBA * AMBIENT_SCALE);

    const std::string twistShader = CFilesystemUtils::LoadFileAsString("res/twist.vert");
    m_programTwist.CompileShader(twistShader, ShaderType::Vertex);
    m_programTwist.Link();

    std::cerr << "-- TWIST program info ---" << std::endl;
    CProgramInfo info = m_programTwist.GetProgramInfo();
    info.PrintProgramInfo(std::cerr);
    std::cerr << "-------------------------" << std::endl;

    if (auto textOpt = m_programTwist.Validate())
    {
        std::cerr << "Validate failed: " << *textOpt << std::endl;
    }
}

void CWindowClient::OnUpdateWindow(float deltaSeconds)
{
    m_twistController.Update(deltaSeconds);
    m_camera.Update(deltaSeconds);
}

void CWindowClient::OnDrawWindow()
{
    SetupView(GetWindow().GetWindowSize());

    m_sunlight.Setup();
    m_lamp.Setup();
    m_umbrellaMat.Setup();

    // Если программа активна, используем её и рисуем поверхность
    // в режиме Wireframe.
    if (m_programEnabled)
    {
        m_programTwist.Use();
        CProgramUniform twist = m_programTwist.FindUniform("TWIST");
        twist = m_twistController.GetCurrentValue();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_umbrellaObj.Draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
        m_programFixed.Use();
        m_umbrellaObj.Draw();
    }
}

void CWindowClient::OnKeyDown(const SDL_KeyboardEvent &event)
{
    if (m_camera.OnKeyDown(event) || m_twistController.OnKeyDown(event))
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
    switch (event.keysym.sym)
    {
    case SDLK_SPACE:
        m_programEnabled = !m_programEnabled;
        break;
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
