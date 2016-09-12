#pragma once
#include "libchapter3.h"

class CEarthProgramContext
{
public:
    CEarthProgramContext();

    void Use();

private:
    CTexture2DUniquePtr m_pEarthTexture;
    CTexture2DUniquePtr m_pCloudTexture;
    CTexture2DUniquePtr m_pNightTexture;
    CShaderProgram m_programEarth;
};
