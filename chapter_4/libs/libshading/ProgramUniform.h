#pragma once
#include <glm/fwd.hpp>
#include <vector>

class CShaderProgram;

class CProgramUniform
{
public:
    explicit CProgramUniform(int location);

    // Отправка скалярных и векторных значений
    void operator =(int value);
    void operator =(float value);
    void operator =(const glm::vec2 &value);
    void operator =(const glm::ivec2 &value);
    void operator =(const glm::vec3 &value);
    void operator =(const glm::vec4 &value);
    void operator =(const glm::mat3 &value);
    void operator =(const glm::mat4 &value);

    // Отправка массива матриц.
    void operator =(const std::vector<glm::mat4> &value);

    // Блокируем случайное использование других типов.
    void operator =(bool) = delete;
    void operator =(double value) = delete;
    void operator =(unsigned value) = delete;
    void operator =(const void *) = delete;

private:
    int m_location = -1;
};
