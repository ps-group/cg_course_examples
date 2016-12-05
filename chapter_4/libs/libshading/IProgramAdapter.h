#pragma once

class CShaderProgram;
class CProgramUniform;
class CVertexAttribute;

// Список стандартных Uniform-переменных, используемых рендерером.
enum class UniformId : unsigned
{
    // Параметры источника света
    LIGHT_POSITION, // vec4
    LIGHT_DIFFUSE, // vec4
    LIGHT_SPECULAR, // vec4
    // Параметры материала
    MATERIAL_DIFFUSE, // vec4
    MATERIAL_SPECULAR, // vec4
    MATERIAL_EMISSIVE, // vec4
    MATERIAL_SHININESS, // float
    TEX_DIFFUSE, // Sampler2D
    TEX_SPECULAR, // Sampler2D
    TEX_EMISSIVE, // Sampler2D
    // Матрицы трансформаций
    MATRIX_VIEW, // mat4
    MATRIX_PROJECTION, // mat4
    MATRIX_WORLDVIEW, // mat4 или mat3
    MATRIX_NORMALWORLDVIEW, // mat4
    // Отдельные элементы преобразований
    TRANSFORM_SCALE, // vec3
    // Массив трансформаций костей.
    BONE_TRANSFORM_ARRAY, // массив mat4[]
};

enum class AttributeId
{
    // Позиция вершины (vec3 или vec2)
    POSITION,
    // Текстурные координаты (vec2)
    TEX_COORD_UV,
    // Нормаль (vec3)
    NORMAL,
    // Тангенциальная касательная (vec3)
    TANGENT,
    // Битангенциальная касательная (vec3)
    BITANGENT,
    // Позиция экземпляра объекта (vec3), см. на тему "OpenGL instancing"
    INSTANCE_POSITION,
    // Индексы костей, влияющих на вершину (uvec4),
    //  используются для скининга в скелетной анимации
    BONE_INDICIES,
    // Веса костей, влияющих на вершину,
    //  используются для скининга в скелетной анимации
    BONE_WEIGHTS,
};

class IProgramAdapter
{
public:
    virtual ~IProgramAdapter() = default;

    virtual void Use()const = 0;
    virtual CProgramUniform GetUniform(UniformId id)const = 0;
    virtual CVertexAttribute GetAttribute(AttributeId id)const = 0;
};
