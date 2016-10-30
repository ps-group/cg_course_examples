#pragma once
#include "libchapter4/VertexAttribute.h"
#include "libchapter4/ComplexMesh.h"
#include <glm/mat4x4.hpp>

class CPlanetProgram;

class CPlanetRenderer3D : public IComplexMeshRenderer
{
public:
    CPlanetRenderer3D(CPlanetProgram &program);
    ~CPlanetRenderer3D();

    void SetWorldTransform(const glm::mat4 &value);

    void SetTransform(const glm::mat4 &transform) override;
    void BindAttribute(Attribute attribute, size_t offset, size_t stride) override;
    void UnbindAttribute(Attribute attribute) override;
    void SetMaterialLayer(Layer layer, CTexture2D *pTexture, const glm::vec4 &color) override;

private:
    CPlanetProgram &m_program;
    CVertexAttribute m_vertexAttr;
    CVertexAttribute m_normalAttr;
    CVertexAttribute m_texCoordAttr;
    glm::mat4 m_worldTransform;
};
