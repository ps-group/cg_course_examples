#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

CTransform3D::CTransform3D()
    : m_sizeScale(glm::vec3(1))
    , m_orientation(glm::angleAxis(0.f, glm::vec3(0, 1, 0)))
{
}

mat4 CTransform3D::ToMat4() const
{
    const mat4 scaleMatrix = scale(mat4(), m_sizeScale);
    const mat4 rotationMatrix = mat4_cast(m_orientation);
    const mat4 translateMatrix = translate(mat4(), m_position);

    return translateMatrix * rotationMatrix * scaleMatrix;
}
