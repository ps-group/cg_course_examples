#pragma once
#include <glm/fwd.hpp>
#include <cstdlib>

/**
 * @class IRenderer3D - интерфейс, устанавливающий связь между
 * классами для вывода вершин и программой на GLSL.
 *
 * Методы интерфейса устанавливают массив атрибутов вершины,
 * используемых позднее для вызовов glDrawElements или glDrawArrays.
 *
 * Параметры методов обеспечивают работу со смешанными массивами:
 *  - offset - смещение (в байтах) от начала буфера данных в видеопамяти
 *             до первого элемента нужного типа данных.
 *  - stride - число байт между двумя элементами одного типа данных
 */
class IRenderer3D
{
public:
    virtual ~IRenderer3D() = default;

    virtual void SetTexCoord2DOffset(size_t offset, size_t stride) = 0;
    virtual void SetPosition3DOffset(size_t offset, size_t stride) = 0;
    virtual void SetNormalOffset(size_t offset, size_t stride) = 0;
};
