#include "includes/opengl-common.hpp"
#include "ProgramInfo.h"
#include <stdexcept>
#include <iostream>


namespace
{
// Преобразует идентификатор типа данных GLSL в строку
std::string TypeToString(GLenum type)
{
    const std::pair<GLenum, const char *> TYPE_MAPPING[] =
    {
        {GL_FLOAT, "float"},
        {GL_FLOAT_VEC2, "vec2"},
        {GL_FLOAT_VEC3, "vec3"},
        {GL_FLOAT_VEC4, "vec4"},
        {GL_INT, "int"},
        {GL_INT_VEC2, "ivec2"},
        {GL_INT_VEC3, "ivec3"},
        {GL_INT_VEC4, "ivec4"},
        {GL_BOOL, "bool"},
        {GL_BOOL_VEC2, "bvec2"},
        {GL_BOOL_VEC3, "bvec3"},
        {GL_BOOL_VEC4, "bvec4"},
        {GL_FLOAT_MAT2, "mat2"},
        {GL_FLOAT_MAT3, "mat3"},
        {GL_FLOAT_MAT4, "mat4"},
        {GL_FLOAT_MAT2x3, "mat2x3"},
        {GL_FLOAT_MAT2x4, "mat2x4"},
        {GL_FLOAT_MAT3x2, "mat3x2"},
        {GL_FLOAT_MAT3x4, "mat3x4"},
        {GL_FLOAT_MAT4x2, "mat4x2"},
        {GL_FLOAT_MAT4x3, "mat4x3"},
        {GL_SAMPLER_1D, "sampler1D"},
        {GL_SAMPLER_2D, "sampler2D"},
        {GL_SAMPLER_3D, "sampler3D"},
        {GL_SAMPLER_CUBE, "samplerCube"},
        {GL_SAMPLER_1D_SHADOW, "sampler1DShadow"},
        {GL_SAMPLER_2D_SHADOW, "sampelr2DShadow"}
    };
    for (const auto &pair : TYPE_MAPPING)
    {
        if (pair.first == type)
        {
            return pair.second;
        }
    }
    throw std::invalid_argument("Unknown variable type " + std::to_string(type));
}
}


int CProgramInfo::GetMaxVertexUniforms()
{
    GLint result;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &result);
    return result;
}

int CProgramInfo::GetMaxFragmentUniforms()
{
    GLint result;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &result);
    return result;
}

CProgramInfo::CProgramInfo(unsigned programId)
    : m_programId(programId)
{
}

unsigned CProgramInfo::GetUniformCount() const
{
    GLint count = 0;
    glGetProgramiv(m_programId, GL_ACTIVE_UNIFORMS, &count);

    return unsigned(count);
}

void CProgramInfo::PrintUniformInfo(unsigned index, std::ostream &stream) const
{
    GLint uniformArraySize = 0;
    GLenum uniformType = 0;
    char nameBuffer[256];
    GLsizei nameLength = 0;
    glGetActiveUniform(m_programId, index, GLsizei(sizeof(nameBuffer)),
                       &nameLength, &uniformArraySize, &uniformType,
                       reinterpret_cast<GLchar *>(nameBuffer));
    std::string name(nameBuffer, size_t(nameLength));
    // Выводим имя и тип переменной.
    stream << TypeToString(uniformType) << " " << name;
    // Если это массив, выводим его размер.
    if (uniformArraySize != 1)
    {
        stream << "[" << uniformArraySize << "]";
    }

    // Если это не встроенная переменная, то выводим ее расположение
    if (name.length() > 3 && name.substr(0, 3) != "gl_")
    {
        GLint location = glGetUniformLocation(m_programId, name.c_str());
        stream << " at " << location;
    }
}

void CProgramInfo::PrintProgramInfo(std::ostream &stream) const
{
    const GLuint uniformCount = GetUniformCount();
    stream << "Program id: " << m_programId << "\n";
    stream << " Active uniform count: " << uniformCount << "\n";
    for (GLuint uniform = 0; uniform < uniformCount; ++uniform)
    {
        stream << "  ";
        PrintUniformInfo(uniform, stream);
        stream << "\n";
    }
}
