#pragma once
#include "libshading/IProgramAdapter.h"
#include "Components.h"
#include <glm/mat4x4.hpp>

class CPlanetProgram;

class CPlanetRenderer3D
{
public:
    CPlanetRenderer3D(const IProgramAdapter &program);
    ~CPlanetRenderer3D();

    void SetupLight0(const glm::vec4 &position,
                     const glm::vec4 &diffuse,
                     const glm::vec4 &specular);
    void SetWorldMat4(const glm::mat4 &value);
    void SetViewMat4(const glm::mat4 &value);
    void SetProjectionMat4(const glm::mat4 &value);
    void Draw(const CMeshComponent &mesh);

private:
    CProgramUniform GetUniform(UniformId id)const;
    void BindTextures(const CMeshComponent &mesh);
    void BindAttributes(const SGeometryLayout &layout)const;

    const IProgramAdapter &m_program;
    glm::mat4 m_world;
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_normal;
};
