#pragma once

#include "MeshP3NT2.h"
#include "libchapter2.h"

class CIdentitySphere final : public ISceneObject
{
public:
    CIdentitySphere(unsigned slices, unsigned stacks);

    void Update(float) final {}
    void Draw()const final;

private:
    void Tesselate(unsigned slices, unsigned stacks);

    SMeshP3NT2 m_mesh;
};
