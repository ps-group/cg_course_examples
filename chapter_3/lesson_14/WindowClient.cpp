#include "stdafx.h"
#include "WindowClient.h"

namespace
{
const glm::vec2 QUAD_TOPLEFT = { -200, -200 };
const glm::vec2 QUAD_SIZE = { 400, 400 };

const char VERTEX_SHADER[] = R"***(
void main()
{
    // Transform the vertex:
    // gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex
    gl_Position = ftransform();
    // Copy texture coordinates from gl_MultiTexCoord0 vertex attribute
    // to gl_TexCoord[0] varying variable
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
)***";

const char FRAGMENT_SHADER_CHECKERS[] = R"***(
void main()
{
    // determine whether fraction part of x and y
    // texture coordinate is greater than 0.5
    vec2 stepXY = step(vec2(0.5, 0.5), fract(gl_TexCoord[0].xy));
    // determine whether the texture coordinate
    // is within a black or white check
    gl_FragColor = vec4((stepXY.x != stepXY.y) ? 1.0 : 0.0);
}
)***";

const char FRAGMENT_SHADER_PICTURE[] = R"***(
// Check if the point p is on the left side of the line p0p1
bool PointIsOnTheLeft(vec2 p0, vec2 p1, vec2 p)
{
    vec2 p0p1 = p1 - p0;
    // find the orthogonal vector to p0p1
    vec2 n = vec2(-p0p1.y, p0p1.x);
    // Find the dot product between n and (p - p0)
    return dot(p - p0, n) > 0.0;
}

bool PointIsInsideTriangle(vec2 p0, vec2 p1, vec2 p2, vec2 p)
{
    return PointIsOnTheLeft(p0, p1, p) &&
           PointIsOnTheLeft(p1, p2, p) &&
           PointIsOnTheLeft(p2, p0, p);
}

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    const vec2 p0 = vec2(1.0, 1.0);
    const vec2 p1 = vec2(3.0, 2.0);
    const vec2 p2 = vec2(2.5, 3.5);
    if (PointIsInsideTriangle(p0, p1, p2, pos))
    {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // determine whether fraction part of x and y
        // texture coordinate is greater than 0.5
        vec2 stepXY = step(vec2(0.5, 0.5), fract(pos));
        // determine whether the texture coordinate
        // is within a black or white check
        gl_FragColor = vec4((stepXY.x != stepXY.y) ? 1.0 : 0.0);
    }
}
)***";

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
    , m_quadObj(QUAD_TOPLEFT, QUAD_SIZE)
{
    const glm::vec4 GREEN = { 0.15f, 0.4f, 0.15f, 1.f };
    GetWindow().SetBackgroundColor(GREEN);
    SetupOpenGLState();

    m_programCheckers.CompileShader(VERTEX_SHADER, ShaderType::Vertex);
    m_programCheckers.CompileShader(FRAGMENT_SHADER_CHECKERS, ShaderType::Fragment);
    m_programCheckers.Link();

    m_programPicture.CompileShader(VERTEX_SHADER, ShaderType::Vertex);
    m_programPicture.CompileShader(FRAGMENT_SHADER_PICTURE, ShaderType::Fragment);
    m_programPicture.Link();

    m_programQueue = { &m_programPicture, &m_programCheckers };
}

void CWindowClient::OnUpdateWindow(float deltaSeconds)
{
    (void)deltaSeconds;
    SetupView(GetWindow().GetWindowSize());

    // Активной будет первая программа из очереди.
    m_programQueue.front()->Use();
    m_quadObj.Draw();
}

void CWindowClient::OnKeyUp(const SDL_KeyboardEvent &event)
{
    // Передвигаем очередь программ,
    // если была нажата и отпущена клавиша "Пробел"
    if (event.keysym.sym == SDLK_SPACE)
    {
        std::rotate(m_programQueue.begin(), m_programQueue.begin() + 1, m_programQueue.end());
    }
}

void CWindowClient::SetupView(const glm::ivec2 &size)
{
    // Матрица ортографического проецирования изображения в трёхмерном пространстве
    // из параллелипипеда с размером, равным (size.X x size.Y x 2).
    const float halfWidth = float(size.x) * 0.5f;
    const float halfHeight = float(size.y) * 0.5f;
    const glm::mat4 matrix = glm::ortho<float>(-halfWidth, halfWidth, -halfHeight, halfHeight);
    glViewport(0, 0, size.x, size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));
    glMatrixMode(GL_MODELVIEW);
}
