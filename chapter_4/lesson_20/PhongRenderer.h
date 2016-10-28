#pragma once
#include "libchapter4/VertexAttribute.h"
#include "libchapter4/ComplexMesh.h"

class CPlanetProgram;

class CPlanetRenderer3D : public IComplexMeshRenderer
{
public:
    CPlanetRenderer3D(CPlanetProgram &context);
    ~CPlanetRenderer3D();

    void BindAttribute(Attribute attribute, size_t offset, size_t stride) override;
    void UnbindAttribute(Attribute attribute) override;
    void BindTexture(Layer layer, CTexture2D *pTexture) override;

private:
    CPlanetProgram &m_context;
    CVertexAttribute m_vertexAttr;
    CVertexAttribute m_normalAttr;
    CVertexAttribute m_texCoordAttr;
};
