#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace glm;

CTransform2D::CTransform2D()
    : m_sizeScale(0, 0)
    , m_rotation(0)
{
}

mat3 CTransform2D::ToMat3() const
{
    const mat3 scaleMatrix = scale(mat3(), m_sizeScale);
    const mat3 rotationMatrix = rotate(mat3(), m_rotation);
    const mat3 translateMatrix = translate(mat3(), m_position);

    return translateMatrix * rotationMatrix * scaleMatrix;
}

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
