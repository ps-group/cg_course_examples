#include "includes/opengl-common.hpp"
#include "includes/glm-common.hpp"
#include "ProgramUniform.h"


CProgramUniform::CProgramUniform(int location)
    : m_location(location)
{
}

void CProgramUniform::operator =(int value)
{
    if (m_location != -1)
    {
        glUniform1i(m_location, value);
    }
}

void CProgramUniform::operator =(float value)
{
    if (m_location != -1)
    {
        glUniform1f(m_location, value);
    }
}

void CProgramUniform::operator =(const glm::vec2 &value)
{
    if (m_location != -1)
    {
        glUniform2fv(m_location, 1, glm::value_ptr(value));
    }
}

void CProgramUniform::operator =(const glm::ivec2 &value)
{
    if (m_location != -1)
    {
        glUniform2iv(m_location, 1, glm::value_ptr(value));
    }
}

void CProgramUniform::operator =(const glm::vec3 &value)
{
    if (m_location != -1)
    {
        glUniform3fv(m_location, 1, glm::value_ptr(value));
    }
}

void CProgramUniform::operator =(const glm::vec4 &value)
{
    if (m_location != -1)
    {
        glUniform4fv(m_location, 1, glm::value_ptr(value));
    }
}

void CProgramUniform::operator =(const glm::mat3 &value)
{
    if (m_location != -1)
    {
        glUniformMatrix3fv(m_location, 1, false, glm::value_ptr(value));
    }
}

void CProgramUniform::operator =(const glm::mat4 &value)
{
    if (m_location != -1)
    {
        glUniformMatrix4fv(m_location, 1, false, glm::value_ptr(value));
    }
}

void CProgramUniform::operator =(const std::vector<glm::mat4> &value)
{
    if ((m_location != -1) && !value.empty())
    {
        const GLsizei count = GLsizei(value.size());
        glUniformMatrix4fv(m_location, count, false, glm::value_ptr(value[0]));
    }
}
