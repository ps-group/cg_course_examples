#include "stdafx.h"
#include "StaticProgramContext.h"

namespace
{
glm::mat4 GetNormalMatrix(const glm::mat4 &modelView)
{
    return glm::transpose(glm::inverse(modelView));
}
}

CStaticProgramContext::CStaticProgramContext()
{
    CTexture2DLoader loader;
    const auto vertShader = CFilesystemUtils::LoadFileAsString("res/static_phong.vert");
    const auto fragShader = CFilesystemUtils::LoadFileAsString("res/static_phong.frag");
    m_programEarth.CompileShader(vertShader, ShaderType::Vertex);
    m_programEarth.CompileShader(fragShader, ShaderType::Fragment);
    m_programEarth.Link();
}

void CStaticProgramContext::Use()
{
    m_programEarth.Use();
    m_programEarth.FindUniform("diffuseMap") = 0; // GL_TEXTURE0
    m_programEarth.FindUniform("specularMap") = 1; // GL_TEXTURE1

    UpdateModelViewProjection();

    m_programEarth.FindUniform("light0.position") = m_light0.position;
    m_programEarth.FindUniform("light0.diffuse") = m_light0.diffuse;
    m_programEarth.FindUniform("light0.specular") = m_light0.specular;
}

CVertexAttribute CStaticProgramContext::GetPositionAttr() const
{
    return m_programEarth.FindAttribute("vertex");
}

CVertexAttribute CStaticProgramContext::GetNormalAttr() const
{
    return m_programEarth.FindAttribute("normal");
}

CVertexAttribute CStaticProgramContext::GetTexCoordAttr() const
{
    return m_programEarth.FindAttribute("textureUV");
}

const glm::mat4 &CStaticProgramContext::GetModel() const
{
    return m_model;
}

const glm::mat4 &CStaticProgramContext::GetView() const
{
    return m_view;
}

const glm::mat4 &CStaticProgramContext::GetProjection() const
{
    return m_projection;
}

const CStaticProgramContext::SLightSource &CStaticProgramContext::GetLight0() const
{
    return m_light0;
}

void CStaticProgramContext::SetModel(const glm::mat4 &value)
{
    m_model = value;
}

void CStaticProgramContext::SetView(const glm::mat4 &value)
{
    m_view = value;
}

void CStaticProgramContext::SetProjection(const glm::mat4 &value)
{
    m_projection = value;
}

void CStaticProgramContext::SetLight0(const SLightSource &source)
{
    m_light0 = source;
}

void CStaticProgramContext::BindDiffuseMap(CTexture2D &texture)
{
    // устанавливаем текстуру для текстурного слота #0
    texture.Bind();
}

void CStaticProgramContext::BindSpecularMap(CTexture2D &texture)
{
    // переключаемся на текстурный слот #1
    glActiveTexture(GL_TEXTURE1);
    texture.Bind();
    // переключаемся обратно на текстурный слот #0
    // перед началом рендеринга активным будет именно этот слот.
    glActiveTexture(GL_TEXTURE0);
}

void CStaticProgramContext::UpdateModelViewProjection()
{
    const glm::mat4 mv = m_view * m_model;
    m_programEarth.FindUniform("view") = m_view;
    m_programEarth.FindUniform("modelView") = mv;
    m_programEarth.FindUniform("normalModelView") = GetNormalMatrix(mv);
    m_programEarth.FindUniform("projection") = m_projection;
}
