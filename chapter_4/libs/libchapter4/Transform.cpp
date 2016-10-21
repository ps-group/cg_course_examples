#include "libchapter4_private.h"
#include "Transform.h"

using namespace glm;

#if 0
glm::mat4 GetTransformMatrix(const CTransformComponent &component)
{
    glm::mat4 result;
    result = glm::translate(result, component.m_position)
            * glm::mat4_cast(component.m_rotation);
    result = glm::scale(result, component.m_scale);

    return result;
}
#endif

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
