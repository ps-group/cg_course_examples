#pragma once
#include "libchapter3.h"
#include "IRenderer3D.h"

class CEarthProgramContext;

class CEarthRenderer3D : public IRenderer3D
{
public:
    CEarthRenderer3D(CEarthProgramContext &context);
    ~CEarthRenderer3D();

    // IRenderer3D interface
    void SetTexCoord2DOffset(size_t offset, size_t stride) override;
    void SetPosition3DOffset(size_t offset, size_t stride) override;
    void SetNormalOffset(size_t offset, size_t stride) override;

private:
    CEarthProgramContext &m_context;
    CVertexAttribute m_vertexAttr;
    CVertexAttribute m_normalAttr;
    CVertexAttribute m_texCoordAttr;
};
