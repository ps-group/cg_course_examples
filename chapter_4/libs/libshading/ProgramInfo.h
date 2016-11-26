#pragma once
#include <string>

class CProgramInfo
{
public:
    static int GetMaxVertexUniforms();
    static int GetMaxFragmentUniforms();

    explicit CProgramInfo(unsigned programId);

    /// Возвращает количество uniform-переменных в активной программе.
    unsigned GetUniformCount()const;

    /// Выводит информацию о uniform-переменной в поток stream.
    void PrintUniformInfo(unsigned index, std::ostream &stream)const;

    /// Выводим информацию о всех uniform-переменных программы.
    void PrintProgramInfo(std::ostream &stream)const;

private:
    unsigned m_programId = 0;
};
