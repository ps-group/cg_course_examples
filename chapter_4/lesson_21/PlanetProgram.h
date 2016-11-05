#pragma once
#include "libshading/ShaderProgram.h"
#include "libshading/AbstractProgramAdapter.h"

class CPlanetProgram : public CAbstractProgramAdapter
{
public:
    CPlanetProgram();

    // CAbstractProgramAdapter interface
protected:
    const CShaderProgram &GetProgram() const override;

private:
    CShaderProgram m_program;
};
