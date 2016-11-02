#pragma once

#include <glm/fwd.hpp>
#include <stddef.h>

enum class VertexAttribute
{
    TexCoord2D,
    Position3D,
    Normal,
};

enum class MaterialLayer
{
    Diffuse,
    Specular,
    Emissive
};

class CTexture2D;
class CBoundingBox;

/**
 * @class IComplexMeshRenderer - интерфейс, устанавливающий связь между
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
class IMeshRenderer
{
public:
    virtual ~IMeshRenderer() = default;

    virtual void SetTransform(const glm::mat4 &transform) = 0;

    /**
     * Параметры обеспечивают работу со смешанными массивами:
     *  - offset - смещение (в байтах) от начала буфера данных в видеопамяти
     *             до первого элемента нужного типа данных.
     *  - stride - число байт между двумя элементами одного типа данных
     **/
    virtual void BindAttribute(VertexAttribute attribute, size_t offset, size_t stride) = 0;

    /// Выполняет отключение вершинного атрибута.
    virtual void UnbindAttribute(VertexAttribute attribute) = 0;

    /// Выполняет привязку текстуры, если указатель не равен нулю,
    ///  иначе отвязывает текстуру и устанавливает вместо неё цвет color.
    virtual void SetMaterialLayer(MaterialLayer layer, CTexture2D *pTexture, const glm::vec4 &color) = 0;

    /// Устанавливает коэффициент разброса бликов материала.
    virtual void ApplyShininess(float shininess) = 0;
};

class IMesh
{
public:
    virtual ~IMesh() = default;

    virtual void Draw(IMeshRenderer &renderer)const = 0;
    virtual CBoundingBox GetBoundingBox()const = 0;
};
