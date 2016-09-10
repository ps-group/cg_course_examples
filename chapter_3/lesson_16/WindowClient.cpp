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
    glEnable(GL_TEXTURE_2D);
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

    window.SetBackgroundColor(BLACK);
    CheckOpenGLVersion();
    SetupOpenGLState();

    std::string colormapPath = CFilesystemUtils::GetResourceAbspath("res/img/earth.bmp");
    m_pEarthTexture = LoadTexture2DFromBMP(colormapPath);

    std::string cloudsPath = CFilesystemUtils::GetResourceAbspath("res/img/earth_clouds.bmp");
    m_pCloudTexture = LoadTexture2DFromBMP(cloudsPath);

    m_sphereMat.SetDiffuse(WHITE_RGBA);
    m_sphereMat.SetAmbient(WHITE_RGBA);
    m_sphereMat.SetSpecular(WHITE_RGBA);
    m_sphereMat.SetShininess(30);

    m_sunlight.SetDirection(SUNLIGHT_DIRECTION);
    m_sunlight.SetDiffuse(WHITE_RGBA);
    m_sunlight.SetAmbient(0.4f * WHITE_RGBA);
    m_sunlight.SetSpecular(WHITE_RGBA);

    const std::string vertexShader = CFilesystemUtils::LoadFileAsString("res/cloud_earth.vert");
    const std::string phongShader = CFilesystemUtils::LoadFileAsString("res/cloud_earth.frag");

    m_programPhong.CompileShader(vertexShader, ShaderType::Vertex);
    m_programPhong.CompileShader(phongShader, ShaderType::Fragment);
    m_programPhong.Link();

    m_programQueue = { &m_programPhong, &m_programFixed };
}

void CWindowClient::OnUpdateWindow(float deltaSeconds)
{
    m_camera.Update(deltaSeconds);
    SetupView(GetWindow().GetWindowSize());

    m_sphereMat.Setup();
    m_sunlight.Setup();

    // переключаемся на текстурный слот #1
    glActiveTexture(GL_TEXTURE1);
    m_pCloudTexture->Bind();
    // переключаемся обратно на текстурный слот #0
    // перед началом рендеринга активным будет именно этот слот.
    glActiveTexture(GL_TEXTURE0);
    m_pEarthTexture->Bind();

    // Активной будет первая программа из очереди.
    const CShaderProgram &program = *m_programQueue.front();
    program.Use();
    program.FindUniform("colormap") = 0; // GL_TEXTURE0
    program.FindUniform("surfaceDataMap") = 1; // GL_TEXTURE1

    m_pEarthTexture->DoWhileBinded([this]{
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
    // Ещё в OpenGL 1.2 мультитекстурирование также вошло в спецификацию,
    // см. http://opengl.org/registry/specs/ARB/multitexture.txt
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
