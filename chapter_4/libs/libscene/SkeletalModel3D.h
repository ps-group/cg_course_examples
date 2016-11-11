#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Geometry.h"
#include "BufferObject.h"
#include "Texture2D.h"

class CSkeletalModel3D;
using CSkeletalModel3DPtr = std::shared_ptr<CSkeletalModel3D>;

class CSkeletalNode;
using CSkeletalNodePtr = std::unique_ptr<CSkeletalNode>;

// Представляет узел модели, который может использоваться как кость.
class CSkeletalNode
{
public:
    // Имя узла (для указания в описании анимации).
    std::string m_name;
    // Матрица перехода из системы координат родительского узла
    //  в систему координат узла.
    glm::mat4 m_localMat4;
    // Список дочерних узлов (возможно, играющих роль костей).
    std::vector<CSkeletalNodePtr> m_children;
};

class CSkeletalMesh3D
{
public:
    // Размещение данных сетки примитивов в памяти модели.
    SGeometryLayout m_layout;
    // Номер материала в материалах модели.
    unsigned m_materialIndex = 0;
    // Список узлов-костей, взаимодействующих с данной сеткой.
    std::vector<const CSkeletalNode*> m_bones;
};

class CSkeletalModel3D
{
public:
    std::vector<SPhongMaterial> m_materials;
    std::vector<CSkeletalMesh3D> m_meshes;
    CSkeletalNodePtr m_rootNode;
    CGeometrySharedPtr m_pGeometry;
};
