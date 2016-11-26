#include "includes/opengl-common.hpp"
#include "ShaderProgram.h"
#include "ProgramUniform.h"
#include "VertexAttribute.h"
#include "ProgramInfo.h"

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

// Указатель на вызов OpenGL, принимающий id программы и имя переменной,
// и возвращающий её значение.
using GetProgramLocationFn = int (GLAPIENTRY *)(unsigned programId, const GLchar *name);

// Запрашивает расположение uniform или attribute переменной
//  по имени, используя переданный вызов API OpenGL.
// Использует переданный кеш для уменьшения числа вызовов API OpenGL.
// Выбрасывает std::runtime_error в случае, если переменной нет в программе.
int GetCachedVariableLocation(unsigned programId,
                              std::map<std::string, int> &cache,
                              GetProgramLocationFn getLocationFn,
                              const std::string &name)
{
    auto cacheIt = cache.find(name);
    int location = 0;

    if (cacheIt != cache.end())
    {
        location = cacheIt->second;
    }
    else
    {
        location = getLocationFn(programId, name.c_str());
        if (location == -1)
        {
            throw std::runtime_error("Wrong shader variable name: " + std::string(name));
        }
        cache[name] = location;
    }

    return location;
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
    // Выполняем detach и delete после полного формирования программы
    // http://gamedev.stackexchange.com/questions/47910
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

CProgramInfo CShaderProgram::GetProgramInfo() const
{
    return CProgramInfo(m_programId);
}

CProgramUniform CShaderProgram::FindUniform(const std::string &name) const
{
    const int location = GetCachedVariableLocation(m_programId,
                                                   m_uniformLocationCache,
                                                   glGetUniformLocation,
                                                   name);
    return CProgramUniform(location);
}

CVertexAttribute CShaderProgram::FindAttribute(const std::string &name) const
{
    const int location = GetCachedVariableLocation(m_programId,
                                                   m_attributeLocationCache,
                                                   glGetAttribLocation,
                                                   name);
    return CVertexAttribute(location);
}

void CShaderProgram::Use() const
{
    glUseProgram(m_programId);
}

void CShaderProgram::FreeShaders()
{
    for (unsigned shaderId : m_shaders)
    {
        glDetachShader(m_programId, shaderId);
        glDeleteShader(shaderId);
    }
    m_shaders.clear();
}
