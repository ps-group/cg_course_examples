#include "stdafx.h"
#include "ParticleProgram.h"

CParticleProgram::CParticleProgram()
{
    CAssetLoader loader;
    const auto vertShader = loader.LoadFileAsString("res/particle_system/particles.vert");
    const auto fragShader = loader.LoadFileAsString("res/particle_system/particles.frag");
    m_program.CompileShader(vertShader, ShaderType::Vertex);
    m_program.CompileShader(fragShader, ShaderType::Fragment);
    m_program.Link();

    // Настраиваем привязку uniform-переменных
    //  к стандартным (для нашего проекта) параметрам рисования.
    UniformNamesMap uniforms = {
        { UniformId::TEX_EMISSIVE, "emissiveMap" },
        { UniformId::MATRIX_PROJECTION, "projection" },
        { UniformId::MATRIX_WORLDVIEW, "modelView" },
    };
    SetUniformNames(uniforms);

    // Настраиваем привязку входных параметров вершинного шейдера
    //  к стандартным (для нашего проекта) атрибутам вершин.
    AttributeNamesMap attributes = {
        { AttributeId::INSTANCE_POSITION, "particlePosition" },
        { AttributeId::TEX_COORD_UV, "textureUV" },
    };
    SetAttributeNames(attributes);
}

const CShaderProgram &CParticleProgram::GetProgram() const
{
    return m_program;
}
