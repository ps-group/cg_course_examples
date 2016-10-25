#pragma once
#include "libchapter4.h"
#include "IRenderer3D.h"

class CPlanetProgram;

class CPlanetRenderer3D : public IRenderer3D
{
public:
    CPlanetRenderer3D(CPlanetProgram &context);
    ~CPlanetRenderer3D();

    // IRenderer3D interface
    void SetTexCoord2DOffset(size_t offset, size_t stride) override;
    void SetPosition3DOffset(size_t offset, size_t stride) override;
    void SetNormalOffset(size_t offset, size_t stride) override;

private:
    CPlanetProgram &m_context;
    CVertexAttribute m_vertexAttr;
    CVertexAttribute m_normalAttr;
    CVertexAttribute m_texCoordAttr;
};
