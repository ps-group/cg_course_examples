#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

// Преобразует координаты из локальных в мировые в следующем порядке:
//  - сначала вершины масштабируются
//  - затем поворачиваются
//  - затем переносятся
class CTransform3D
{
public:
    // Преобразует исходную трансформацию в матрицу 4x4.
    glm::mat4 ToMat4()const;

    glm::vec3 m_scale = glm::vec3(1);
    glm::quat m_rotation;
    glm::vec3 m_position;
};
