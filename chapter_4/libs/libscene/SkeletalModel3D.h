#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Geometry.h"
#include "BufferObject.h"
#include "Texture2D.h"

class CSkeletalModel3D;
using CSkeletalModel3DPtr = std::shared_ptr<CSkeletalModel3D>;

class CSkeletalMesh3D
{
public:
    // Размещение данных сетки примитивов в памяти модели.
    SGeometryLayout m_layout;
    // Локальное преобразование из координат сетки в координаты модели.
    glm::mat4 m_local;
    // Номер материала в материалах модели.
    unsigned m_materialIndex = 0;
};

class CSkeletalModel3D
{
public:
    std::vector<SPhongMaterial> m_materials;
    std::vector<CSkeletalMesh3D> m_meshes;
    CGeometrySharedPtr m_pGeometry;
};
