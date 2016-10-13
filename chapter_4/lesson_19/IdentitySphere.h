#pragma once

#include <functional>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "MeshP3NT2.h"

class CIdentitySphere
{
public:
    CIdentitySphere(unsigned slices, unsigned stacks);
    void Draw(IRenderer3D &renderer)const;

private:
    CMeshP3NT2 m_mesh;
};
