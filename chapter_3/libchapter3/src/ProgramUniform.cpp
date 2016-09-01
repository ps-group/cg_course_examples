#include "libchapter3_private.h"
#include "ProgramUniform.h"


CProgramUniform::CProgramUniform(int location)
    : m_location(location)
{
}

void CProgramUniform::operator =(int value)
{
    glUniform1i(m_location, value);
}

void CProgramUniform::operator =(float value)
{
    glUniform1f(m_location, value);
}

void CProgramUniform::operator =(const glm::vec2 &value)
{
    glUniform2fv(m_location, 1, glm::value_ptr(value));
}

void CProgramUniform::operator =(const glm::ivec2 &value)
{
    glUniform2iv(m_location, 1, glm::value_ptr(value));
}

void CProgramUniform::operator =(const glm::vec3 &value)
{
    glUniform3fv(m_location, 1, glm::value_ptr(value));
}

void CProgramUniform::operator =(const glm::vec4 &value)
{
    glUniform4fv(m_location, 1, glm::value_ptr(value));
}

void CProgramUniform::operator =(const glm::mat3 &value)
{
    glUniformMatrix3fv(m_location, 1, false, glm::value_ptr(value));
}

void CProgramUniform::operator =(const glm::mat4 &value)
{
    glUniformMatrix4fv(m_location, 1, false, glm::value_ptr(value));
}
