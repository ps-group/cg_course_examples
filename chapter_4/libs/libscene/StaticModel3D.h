#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Geometry.h"
#include "BufferObject.h"
#include "Texture2D.h"

class CStaticModel3D;
using CStaticModel3DPtr = std::shared_ptr<CStaticModel3D>;

class CStaticMesh3D
{
public:
    // Размещение данных сетки примитивов в памяти модели.
    SGeometryLayout m_layout;
    // Локальное преобразование из координат сетки в координаты модели.
    glm::mat4 m_local;
    // Номер материала в материалах модели.
    unsigned m_materialIndex = 0;
};

class CStaticModel3D
{
public:
    std::vector<SPhongMaterial> m_materials;
    std::vector<CStaticMesh3D> m_meshes;
    CGeometrySharedPtr m_pGeometry;
};
