#include "libchapter3_private.h"
#include "ShaderProgram.h"

namespace
{

class CShaderRaii : private boost::noncopyable
{
public:
    CShaderRaii(ShaderType shaderType)
    {
        m_id = glCreateShader(MapShaderType(shaderType));
    }

    ~CShaderRaii()
    {
        glDeleteShader(m_id);
    }

    operator GLuint()const
    {
        return m_id;
    }

    GLuint Release()
    {
        const GLuint id = m_id;
        m_id = 0;
        return id;
    }

private:
    GLenum MapShaderType(ShaderType shaderType)const
    {
        switch (shaderType)
        {
        case ShaderType::Vertex:
            return GL_VERTEX_SHADER;
        case ShaderType::Fragment:
            return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry:
            // OpenGL 3.2
            return GL_GEOMETRY_SHADER;
        case ShaderType::TessEvaluation:
            // GL_ARB_tessellation_shader or OpenGL 4.0
            return GL_TESS_EVALUATION_SHADER;
        case ShaderType::Compute:
            // GL_ARB_compute_shader or OpenGL 4.3
            return GL_COMPUTE_SHADER;
        }
        throw std::logic_error("Unknown shader type");
    }

    GLuint m_id;
};

// Чтобы не делать различий между Shader/Program,
// передаём указатели на функции OpenGL.
std::string GetInfoLog(GLuint shaderId, PFNGLGETSHADERIVPROC getShaderivProc,
                       PFNGLGETSHADERINFOLOGPROC getShaderInfoLogProc)
{
    GLsizei infoLogLength = 0;
    getShaderivProc(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::string infoLog(size_t(infoLogLength), '\0');
    getShaderInfoLogProc(shaderId, infoLogLength, &infoLogLength, &infoLog[0]);

    // Обрезаем лог до его реальной длины
    if (size_t(infoLogLength) < infoLog.length())
    {
        infoLog.erase(infoLog.begin() + ptrdiff_t(infoLogLength), infoLog.end());
    }

    return infoLog;
}

}


CShaderProgram::CShaderProgram()
    : m_programId(glCreateProgram())
{
}

CShaderProgram::CShaderProgram(fixed_pipeline_t)
    : m_programId(0)
{
}

CShaderProgram::~CShaderProgram()
{
    FreeShaders();
    glDeleteProgram(m_programId);
}

void CShaderProgram::CompileShader(const std::string &source, ShaderType type)
{
    const char *pSourceLines[] = { source.c_str() };
    const GLint pSourceLengths[] = { GLint(source.size()) };

    CShaderRaii shader(type);
    glShaderSource(shader, 1, pSourceLines, pSourceLengths);
    glCompileShader(shader);

    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        const auto log = GetInfoLog(shader, glGetShaderiv, glGetShaderInfoLog);
        throw std::runtime_error("Shader compiling failed: " + log);
    }

    m_shaders.emplace_back(shader.Release());
    glAttachShader(m_programId, m_shaders.back());
}

void CShaderProgram::Link()
{
    glLinkProgram(m_programId);
    GLint linkStatus = 0;
    glGetProgramiv(m_programId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        const auto log = GetInfoLog(m_programId, glGetProgramiv, glGetProgramInfoLog);
        throw std::runtime_error("Program linking failed: " + log);
    }
    FreeShaders();
}

boost::optional<std::string> CShaderProgram::Validate()const
{
    glValidateProgram(m_programId);
    GLint status = 0;
    glGetProgramiv(m_programId, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        const auto log = GetInfoLog(m_programId, glGetProgramiv, glGetProgramInfoLog);
        return log;
    }
    return boost::none;
}

void CShaderProgram::Use() const
{
    glUseProgram(m_programId);
}

void CShaderProgram::UseFixedPipeline()
{
    glUseProgram(0);
}

// Выполняем detach и delete после полного формирования программы
// http://gamedev.stackexchange.com/questions/47910
void CShaderProgram::FreeShaders()
{
    for (unsigned shaderId : m_shaders)
    {
        glDetachShader(m_programId, shaderId);
        glDeleteShader(shaderId);
    }
    m_shaders.clear();
}
