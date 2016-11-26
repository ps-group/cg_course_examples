#include "stdafx.h"
#include "PhongProgram.h"

CPhongProgram::CPhongProgram()
{
    CAssetLoader loader;
    const auto vertShader = loader.LoadFileAsString("res/static_scene/phong.vert");
    const auto fragShader = loader.LoadFileAsString("res/static_scene/phong.frag");
    m_program.CompileShader(vertShader, ShaderType::Vertex);
    m_program.CompileShader(fragShader, ShaderType::Fragment);
    m_program.Link();

    // Настраиваем привязку uniform-переменных
    //  к стандартным (для нашего проекта) параметрам рисования.
    UniformNamesMap uniforms = {
        { UniformId::LIGHT_POSITION, "light0.position" },
        { UniformId::LIGHT_DIFFUSE, "light0.diffuse" },
        { UniformId::LIGHT_SPECULAR, "light0.specular" },
        { UniformId::TEX_DIFFUSE, "diffuseMap" },
        { UniformId::TEX_SPECULAR, "specularMap" },
        { UniformId::TEX_EMISSIVE, "emissiveMap" },
        { UniformId::MATERIAL_SHININESS, "material.shininess" },
        { UniformId::MATERIAL_DIFFUSE, "material.diffuse" },
        { UniformId::MATERIAL_SPECULAR, "material.specular" },
        { UniformId::MATERIAL_EMISSIVE, "material.emissive" },
        { UniformId::MATRIX_VIEW, "view" },
        { UniformId::MATRIX_PROJECTION, "projection" },
        { UniformId::MATRIX_WORLDVIEW, "modelView" },
        { UniformId::MATRIX_NORMALWORLDVIEW, "normalModelView" },
    };
    SetUniformNames(uniforms);

    // Настраиваем привязку входных параметров вершинного шейдера
    //  к стандартным (для нашего проекта) атрибутам вершин.
    AttributeNamesMap attributes = {
        { AttributeId::POSITION, "vertex" },
        { AttributeId::NORMAL, "normal" },
        { AttributeId::TEX_COORD_UV, "textureUV" },
    };
    SetAttributeNames(attributes);
}

const CShaderProgram &CPhongProgram::GetProgram() const
{
    return m_program;
}
