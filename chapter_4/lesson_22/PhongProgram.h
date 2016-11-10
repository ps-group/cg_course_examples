#pragma once
#include "libshading/ShaderProgram.h"
#include "libshading/AbstractProgramAdapter.h"

class CPhongProgram : public CAbstractProgramAdapter
{
public:
    CPhongProgram();

    // CAbstractProgramAdapter interface
protected:
    const CShaderProgram &GetProgram() const override;

private:
    CShaderProgram m_program;
};
