#pragma once
#include "libchapter4.h"
#include "IRenderer3D.h"

class CStaticProgramContext;

class CStaticRenderer3D : public IRenderer3D
{
public:
    CStaticRenderer3D(CStaticProgramContext &context);
    ~CStaticRenderer3D();

    // IRenderer3D interface
    void SetTexCoord2DOffset(size_t offset, size_t stride) override;
    void SetPosition3DOffset(size_t offset, size_t stride) override;
    void SetNormalOffset(size_t offset, size_t stride) override;

private:
    CStaticProgramContext &m_context;
    CVertexAttribute m_vertexAttr;
    CVertexAttribute m_normalAttr;
    CVertexAttribute m_texCoordAttr;
};
