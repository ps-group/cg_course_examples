#include "stdafx.h"
#include "Skybox.h"
#include <stdint.h>

namespace
{
const char TEXTURE_ATLAS[] = "galaxy/galaxy.plist";
}


CSkybox::CSkybox()
{
}

void CSkybox::Draw() const
{
    m_pTexture->DoWhileBinded([this] {
        // Инвертируем передние и задние грани, потому что
        // на поверхность SkyBox мы всегда смотрим изнутри.
        glFrontFace(GL_CW);
        m_cube.Draw();
        glFrontFace(GL_CCW);
    });
}
