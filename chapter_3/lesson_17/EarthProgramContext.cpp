#include "stdafx.h"
#include "EarthProgramContext.h"

namespace
{
glm::mat4 GetNormalMatrix(const glm::mat4 &modelView)
{
    return glm::transpose(glm::inverse(modelView));
}
}

CEarthProgramContext::CEarthProgramContext()
{
    CTexture2DLoader loader;

    m_pEarthTexture = loader.Load("res/img/earth_colormap.jpg");
    m_pCloudTexture = loader.Load("res/img/earth_clouds.jpg");
    m_pNightTexture = loader.Load("res/img/earth_at_night.jpg");

    const auto vertShader = CFilesystemUtils::LoadFileAsString("res/cloud_earth_robust.vert");
    const auto fragShader = CFilesystemUtils::LoadFileAsString("res/cloud_earth_robust.frag");
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

    const glm::mat4 mv = m_view * m_model;
    m_programEarth.FindUniform("view") = m_view;
    m_programEarth.FindUniform("modelView") = mv;
    m_programEarth.FindUniform("normalModelView") = GetNormalMatrix(mv);
    m_programEarth.FindUniform("projection") = m_projection;

    m_programEarth.FindUniform("light0.position") = m_light0.position;
    m_programEarth.FindUniform("light0.diffuse") = m_light0.diffuse;
    m_programEarth.FindUniform("light0.specular") = m_light0.specular;
}

CVertexAttribute CEarthProgramContext::GetPositionAttr() const
{
    return m_programEarth.FindAttribute("vertex");
}

CVertexAttribute CEarthProgramContext::GetNormalAttr() const
{
    return m_programEarth.FindAttribute("normal");
}

CVertexAttribute CEarthProgramContext::GetTexCoordAttr() const
{
    return m_programEarth.FindAttribute("textureUV");
}

const glm::mat4 &CEarthProgramContext::GetModel() const
{
    return m_model;
}

const glm::mat4 &CEarthProgramContext::GetView() const
{
    return m_view;
}

const glm::mat4 &CEarthProgramContext::GetProjection() const
{
    return m_projection;
}

const CEarthProgramContext::SLightSource &CEarthProgramContext::GetLight0() const
{
    return m_light0;
}

void CEarthProgramContext::SetModel(const glm::mat4 &value)
{
    m_model = value;
}

void CEarthProgramContext::SetView(const glm::mat4 &value)
{
    m_view = value;
}

void CEarthProgramContext::SetProjection(const glm::mat4 &value)
{
    m_projection = value;
}

void CEarthProgramContext::SetLight0(const CEarthProgramContext::SLightSource &source)
{
    m_light0 = source;
}
