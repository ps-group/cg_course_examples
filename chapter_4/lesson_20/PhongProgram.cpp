#include "stdafx.h"
#include "PhongProgram.h"

namespace
{
glm::mat4 GetNormalMatrix(const glm::mat4 &modelView)
{
    return glm::transpose(glm::inverse(modelView));
}
}

CPlanetProgram::CPlanetProgram()
{
    CAssetLoader loader;
    const auto vertShader = loader.LoadFileAsString("res/solar_system/planet.vert");
    const auto fragShader = loader.LoadFileAsString("res/solar_system/planet.frag");
    m_program.CompileShader(vertShader, ShaderType::Vertex);
    m_program.CompileShader(fragShader, ShaderType::Fragment);
    m_program.Link();
}

void CPlanetProgram::Use()
{
    m_program.Use();
    m_program.FindUniform("diffuseMap") = 0; // GL_TEXTURE0
    m_program.FindUniform("specularMap") = 1; // GL_TEXTURE1
    m_program.FindUniform("emissiveMap") = 2; // GL_TEXTURE2

    UpdateModelViewProjection();

    m_program.FindUniform("light0.position") = m_light0.position;
    m_program.FindUniform("light0.diffuse") = m_light0.diffuse;
    m_program.FindUniform("light0.specular") = m_light0.specular;
}

CVertexAttribute CPlanetProgram::GetPositionAttr() const
{
    return m_program.FindAttribute("vertex");
}

CVertexAttribute CPlanetProgram::GetNormalAttr() const
{
    return m_program.FindAttribute("normal");
}

CVertexAttribute CPlanetProgram::GetTexCoordAttr() const
{
    return m_program.FindAttribute("textureUV");
}

const glm::mat4 &CPlanetProgram::GetModel() const
{
    return m_model;
}

const glm::mat4 &CPlanetProgram::GetView() const
{
    return m_view;
}

const glm::mat4 &CPlanetProgram::GetProjection() const
{
    return m_projection;
}

const CPlanetProgram::SLightSource &CPlanetProgram::GetLight0() const
{
    return m_light0;
}

void CPlanetProgram::SetModel(const glm::mat4 &value)
{
    m_model = value;
}

void CPlanetProgram::SetView(const glm::mat4 &value)
{
    m_view = value;
}

void CPlanetProgram::SetProjection(const glm::mat4 &value)
{
    m_projection = value;
}

void CPlanetProgram::SetLight0(const SLightSource &source)
{
    m_light0 = source;
}

void CPlanetProgram::BindDiffuseMap(CTexture2D &texture)
{
    // устанавливаем текстуру для текстурного слота #0
    texture.Bind();
}

void CPlanetProgram::BindSpecularMap(CTexture2D &texture)
{
    // переключаемся на текстурный слот #1
    glActiveTexture(GL_TEXTURE1);
    texture.Bind();
    // переключаемся обратно на текстурный слот #0
    // перед началом рендеринга активным будет именно этот слот.
    glActiveTexture(GL_TEXTURE0);
}

void CPlanetProgram::BindEmissiveMap(CTexture2D &texture)
{
    // переключаемся на текстурный слот #2
    glActiveTexture(GL_TEXTURE2);
    texture.Bind();
    // переключаемся обратно на текстурный слот #0
    // перед началом рендеринга активным будет именно этот слот.
    glActiveTexture(GL_TEXTURE0);
}

void CPlanetProgram::UpdateModelViewProjection()
{
    const glm::mat4 mv = m_view * m_model;
    m_program.FindUniform("view") = m_view;
    m_program.FindUniform("modelView") = mv;
    m_program.FindUniform("normalModelView") = GetNormalMatrix(mv);
    m_program.FindUniform("projection") = m_projection;
}
