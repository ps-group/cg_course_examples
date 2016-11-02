#pragma once
#include "libchapter4.h"

class CPlanetProgram
{
public:
    struct SLightSource
    {
        glm::vec4 position;
        glm::vec4 diffuse;
        glm::vec4 specular;
    };

    CPlanetProgram();

    void Use();
    CVertexAttribute GetPositionAttr()const;
    CVertexAttribute GetNormalAttr()const;
    CVertexAttribute GetTexCoordAttr()const;

    const glm::mat4 &GetModel()const;
    const glm::mat4 &GetView()const;
    const glm::mat4 &GetProjection()const;
    const SLightSource &GetLight0()const;

    void SetModel(const glm::mat4 &value);
    void SetView(const glm::mat4 &value);
    void SetProjection(const glm::mat4 &value);
    void SetLight0(const SLightSource &source);
    void SetMaterial(const SMaterial &material);

    // Если текстуры нет, её заменяет цвет материала.
    void BindDiffuseMap(CTexture2D *pTexture, const glm::vec4 &defaultColor);
    void BindSpecularMap(CTexture2D *pTexture, const glm::vec4 &defaultColor);
    void BindEmissiveMap(CTexture2D *pTexture, const glm::vec4 &defaultColor);

    void ApplyShininess(float shininess);
    void UpdateModelViewProjection();

private:
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_normalModelView;
    SLightSource m_light0;
    CShaderProgram m_program;
};
