#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Geometry.h"
#include "BufferObject.h"
#include "Texture2D.h"
#include "../libgeometry/Transform.h"

class CSkeletalModel3D;
using CSkeletalModel3DPtr = std::shared_ptr<CSkeletalModel3D>;

class CSkeletalNode;
using CSkeletalNodePtr = std::unique_ptr<CSkeletalNode>;

// Представляет узел модели, который может использоваться как
//  источник трансформаций для костей или сеток.
class CSkeletalNode
{
public:
    // Имя узла (для указания в описании анимации).
    std::string m_name;
    // Трансформация данного узла,
    //  задающая переход в координаты родительского узла.
    CTransform3D m_transform;
    // Список дочерних узлов графа сцены.
    std::vector<CSkeletalNodePtr> m_children;
};

class CSkeletalBone
{
public:
    // Матрица перехода из системы координат сетки треугольников
    //  в систему координат узла, описывающего кость.
    glm::mat4 m_boneOffset;

    // Указатель на узел, представляющий кость
    //  и её зависимости от других костей.
    const CSkeletalNode *m_pNode = nullptr;
};

class CSkeletalMesh3D
{
public:
    // Размещение данных сетки примитивов в памяти модели.
    SGeometryLayout m_layout;
    // Номер материала в материалах модели.
    unsigned m_materialIndex = 0;
    // Список узлов-костей, взаимодействующих с данной сеткой.
    std::vector<CSkeletalBone> m_bones;
};

class CNodeAnimation
{
public:
    template <class T>
    struct Keyframe
    {
        double time;
        T value;
    };

    // Типы для хранения ключевых точек на шкале времени.
    using Vec3Keyframe = Keyframe<glm::vec3>;
    using QuatKeyframe = Keyframe<glm::quat>;

    // Узел, на который действует анимация.
    CSkeletalNode *m_pNode = nullptr;

    // Значения размера, ориентации и позиции в ключевые моменты времени,
    //  переназначают собственную трансформацию узла.
    // Ключевые точки отсортированы в хронологическом порядке.
    std::vector<Vec3Keyframe> m_positionKeyframes;
    std::vector<QuatKeyframe> m_rotationKeyframes;
    std::vector<Vec3Keyframe> m_scaleKeyframes;
};

// FIXME: поддержка прямой анимации полигональных сеток не реализована.
// FIXME: свойства группы aiAnimBehaviour не сохраняются при загрузке.
class CModelAnimation
{
public:
    std::string m_name;
    double m_duration;
    double m_ticksPerSecond;
    std::vector<CNodeAnimation> m_channels;
};

class CSkeletalModel3D
{
public:
    std::vector<SPhongMaterial> m_materials;
    std::vector<CSkeletalMesh3D> m_meshes;
    std::vector<CModelAnimation> m_animations;
    CSkeletalNodePtr m_rootNode;
    CGeometrySharedPtr m_pGeometry;
};
