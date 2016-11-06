#pragma once
#include "libshading/ShaderProgram.h"
#include "libshading/AbstractProgramAdapter.h"

class CParticleProgram : public CAbstractProgramAdapter
{
public:
    CParticleProgram();

    // CAbstractProgramAdapter interface
protected:
    const CShaderProgram &GetProgram() const override;

private:
    CShaderProgram m_program;
};
