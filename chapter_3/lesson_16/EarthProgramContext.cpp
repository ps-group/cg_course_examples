#include "stdafx.h"
#include "EarthProgramContext.h"

CEarthProgramContext::CEarthProgramContext()
{
    CTexture2DLoader loader;

    m_pEarthTexture = loader.Load("res/img/earth_colormap.jpg");
    m_pCloudTexture = loader.Load("res/img/earth_clouds.jpg");
    m_pNightTexture = loader.Load("res/img/earth_at_night.jpg");

    const std::string vertShader = CFilesystemUtils::LoadFileAsString("res/cloud_earth.vert");
    const std::string fragShader = CFilesystemUtils::LoadFileAsString("res/cloud_earth.frag");
    m_programEarth.CompileShader(vertShader, ShaderType::Vertex);
    m_programEarth.CompileShader(fragShader, ShaderType::Fragment);
    m_programEarth.Link();
}

void CEarthProgramContext::Use()
{
    // переключаемся на текстурный слот #2
    glActiveTexture(GL_TEXTURE2);
    m_pNightTexture->Bind();
    // переключаемся на текстурный слот #1
    glActiveTexture(GL_TEXTURE1);
    m_pCloudTexture->Bind();
    // переключаемся обратно на текстурный слот #0
    // перед началом рендеринга активным будет именно этот слот.
    glActiveTexture(GL_TEXTURE0);
    m_pEarthTexture->Bind();

    m_programEarth.Use();
    m_programEarth.FindUniform("colormap") = 0; // GL_TEXTURE0
    m_programEarth.FindUniform("surfaceDataMap") = 1; // GL_TEXTURE1
    m_programEarth.FindUniform("nightColormap") = 2; // GL_TEXTURE2
}
