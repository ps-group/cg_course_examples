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

    // Патаметр needClamp - отвечает за покомпонентное нормирование
    //   атрибутов к `1.0`, что может пригодиться для передаче цвета (color).
    void SetVec3Offset(size_t offset, size_t stride, bool needClamp);

    void SetVec2Offset(size_t offset, size_t stride);

    // Патаметр needClamp - отвечает за покомпонентное нормирование
    //   атрибутов к `1.0`, что может пригодиться для передаче цвета (color).
    void SetOffset(size_t offset, size_t stride, unsigned numComponents, bool needClamp);

private:
    int m_location = -1;
};
