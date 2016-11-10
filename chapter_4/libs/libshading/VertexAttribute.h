#pragma once
#include <glm/fwd.hpp>
#include <cstdlib>
#include <cstdint>

class CShaderProgram;

class CVertexAttribute
{
public:
    explicit CVertexAttribute(int location);

    bool IsValid()const;

    void EnablePointer();
    void DisablePointer();

    // Устанавливает частоту наложения атрибута в решиме "draw instanced"
    //  - 0 (по умолчанию) - атрибут сопутствует каждой вершине
    //  - 1 (по умолчанию) - атрибут меняется только при смене экземпляра
    void SetDivisor(unsigned divisor);

    // Устанавливает смещение атрибута типа vec3
    // Параметр needClamp - отвечает за покомпонентное нормирование
    //   атрибутов к `1.0`, что может быть полезным для передачи цвета (color).
    void SetVec3Offset(size_t offset, size_t stride, bool needClamp);

    // Устанавливает смещение атрибута типа vec2
    void SetVec2Offset(size_t offset, size_t stride);

    // Устанавливает смещение атрибута типа `float[numComponents]`
    // Параметр needClamp - отвечает за покомпонентное нормирование
    //   атрибутов к `1.0`, что может быть полезным для передачи цвета (color).
    void SetFloatsOffset(size_t offset, size_t stride, unsigned numComponents, bool needClamp);

    // Устанавливает смещение атрибута типа `uint8_t[numComponents]`
    void SetUint8Offset(size_t offset, size_t stride, unsigned numComponents);

private:
    int m_location = -1;
};
