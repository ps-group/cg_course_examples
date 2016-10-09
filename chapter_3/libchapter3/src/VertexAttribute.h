#pragma once
#include <glm/fwd.hpp>
#include <cstdlib>
#include <cstdint>

class CShaderProgram;

class CVertexAttribute
{
public:
    explicit CVertexAttribute(int location);

    void EnablePointer();
    void DisablePointer();

    // Патаметр normalized - отвечает за покомпонентное нормирование
    //   атрибутов к `1.0`, что может пригодиться для передачи цвета (color).
    void SetVec3Offset(size_t offset, size_t stride, bool needClamp);

    void SetVec2Offset(size_t offset, size_t stride);

private:
    int m_location = -1;
};
