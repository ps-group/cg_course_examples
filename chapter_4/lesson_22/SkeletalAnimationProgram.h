#pragma once
#include "libshading/ShaderProgram.h"
#include "libshading/AbstractProgramAdapter.h"

class CSkeletalAnimationProgram : public CAbstractProgramAdapter
{
public:
    CSkeletalAnimationProgram();

    // CAbstractProgramAdapter interface
protected:
    const CShaderProgram &GetProgram() const override;

private:
    CShaderProgram m_program;
};
