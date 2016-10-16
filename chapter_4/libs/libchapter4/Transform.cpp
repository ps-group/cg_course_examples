#include "libchapter4_private.h"
#include "Transform.h"

glm::mat4 CTransform3D::ToMat4() const
{
    glm::mat4 result;
    result = glm::translate(result, m_position)
            * glm::mat4_cast(m_rotation);
    result = glm::scale(result, m_scale);

    return result;
}
