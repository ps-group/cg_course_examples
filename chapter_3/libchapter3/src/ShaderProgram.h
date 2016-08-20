#pragma once
#include <boost/noncopyable.hpp>
#include <boost/scope_exit.hpp>
#include <boost/optional.hpp>
#include <string>
#include <memory>

enum class ShaderType
{
    Vertex,
    Fragment,
    Geometry,
    TessEvaluation,
    Compute,
};

class CShaderProgram : private boost::noncopyable
{
public:
    // Используется как тег для псевдо-программы,
    // переключающей на Fixed Pipeline.
    struct fixed_pipeline_t {};

    CShaderProgram();
    CShaderProgram(fixed_pipeline_t);
    ~CShaderProgram();

    void CompileShader(const std::string &source, ShaderType type)const;
    void Link()const;

    // Валидация - необязательный этап, который может сообщить
    // о проблемах производительности или предупреждениях компилятора GLSL
    boost::optional<std::string> Validate()const;

    void Use()const;
    static void UseFixedPipeline();

    template <class TFunction>
    void DoWithProgram(TFunction && fn)const
    {
        Use();
        // При выходе из функции надо обязательно сделать Unbind.
        BOOST_SCOPE_EXIT_ALL() {
            UseFixedPipeline();
        };
        fn();
    }

private:
    unsigned m_programId = 0;
};
