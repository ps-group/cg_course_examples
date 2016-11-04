#pragma once
#include <boost/noncopyable.hpp>
#include <boost/scope_exit.hpp>
#include <boost/optional.hpp>
#include <string>
#include <memory>
#include <vector>
#include <map>

enum class ShaderType
{
    Vertex,
    Fragment,
    Geometry,
    TessEvaluation,
    Compute,
};

class CProgramInfo;
class CProgramUniform;
class CVertexAttribute;

class CShaderProgram : private boost::noncopyable
{
public:
    // Используется как тег для псевдо-программы,
    // переключающей на Fixed Pipeline.
    struct fixed_pipeline_t {};

    CShaderProgram();
    CShaderProgram(fixed_pipeline_t);
    ~CShaderProgram();

    void CompileShader(const std::string &source, ShaderType type);
    void Link();

    // Валидация - необязательный этап, который может сообщить
    // о проблемах производительности или предупреждениях компилятора GLSL
    boost::optional<std::string> Validate()const;

    CProgramInfo GetProgramInfo()const;
    CProgramUniform FindUniform(const std::string &name)const;
    CVertexAttribute FindAttribute(const std::string &name)const;
    void Use()const;

private:
    void FreeShaders();

    unsigned m_programId = 0;
    std::vector<unsigned> m_shaders;
    mutable std::map<std::string, int> m_uniformLocationCache;
    mutable std::map<std::string, int> m_attributeLocationCache;
};
