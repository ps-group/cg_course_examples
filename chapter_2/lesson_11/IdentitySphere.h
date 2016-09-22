#pragma once

#include "IBody.h"
#include "MeshP3NT2.h"
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class CIdentitySphere final : public IBody
{
public:
    CIdentitySphere(unsigned slices, unsigned stacks);

    void Update(float) final {}
    void Draw()const final;

private:
    void Tesselate(unsigned slices, unsigned stacks);

    SMeshP3NT2 m_mesh;
};
