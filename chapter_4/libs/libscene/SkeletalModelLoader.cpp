#include "SkeletalModelLoader.h"
#include "AssetLoader.h"
#include "AssimpUtils.h"

using boost::filesystem::path;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

namespace
{
const size_t BONES_PER_VERTEX = Limits::BONES_PER_VERTEX;
const size_t MAX_BONES_COUNT = Limits::MAX_BONES_COUNT;
const float EPSILON = std::numeric_limits<float>::epsilon();

// Возвращает количество байт, занятых массивом.
template<class T>
size_t GetBytesCount(const std::vector<T> &data)
{
    return sizeof(T) * data.size();
}

// Добавляет в массив `count` элементов и возвращает указатель на первый из них.
template<class T>
T *AddItemsToWrite(std::vector<T> &data, size_t count)
{
    const size_t oldSize = data.size();
    data.resize(oldSize + count);

    return (data.data() + oldSize);
}

// Хранит информацию о применении костей к одной вершине.
struct CVertexSkinning
{
    uint8_t indexes[BONES_PER_VERTEX];
    float weights[BONES_PER_VERTEX];

    // Конструктор заполняет структуру нулями.
    CVertexSkinning()
    {
        std::memset(indexes, 0, sizeof(indexes));
        std::memset(weights, 0, sizeof(weights));
    }

    // Добавляет вес и индекс кости, от которой зависит вершина.
    void AddWeight(unsigned boneId, float weight)
    {
        assert(weight > EPSILON);

        // Индекс кости сохраняется в uint8_t,
        //  поэтому более 256 костей в модели быть не должно.
        if (boneId > MAX_BONES_COUNT)
        {
            throw std::runtime_error("Implementation doesn't support more than "
                                     + std::to_string(MAX_BONES_COUNT)
                                     + " bones");
        }

        for (size_t i = 0; i < BONES_PER_VERTEX; ++i)
        {
            // Ищем кость с нулевым весом и заполняем её.
            if (weights[i] <= EPSILON)
            {
                weights[i] = weight;
                indexes[i] = uint8_t(boneId);
                return;
            }
        }

        // Если вершина уже зависит от максимального числа костей,
        //  бросаем исключение.
        throw std::runtime_error("Implementation doesn't support more than "
                                 + std::to_string(BONES_PER_VERTEX)
                                 + "bones per vertex");
    }
};

// Накапливает информацию о сетках треугольников и геометрии,
//  а также об анимациях и скелете модели.
class CMeshAccumulator
{
public:
    static const size_t RESERVED_SIZE = 4 * 1024;
    static const unsigned TRI_VERTEX_COUNT = 3;

    CMeshAccumulator()
    {
        m_geometry.m_indicies.reserve(RESERVED_SIZE);
        m_geometry.m_vertexData.reserve(RESERVED_SIZE);
    }

    // Обходит все узлы сцены и формирует Axis Aligned Bounding Box
    void CollectBoundingBox(const aiScene& scene)
    {
        m_geometry.m_bbox = GetNodeBBox(scene, *scene.mRootNode);
    }

    // Обходит все узлы сцены и воссоздаёт иерархию узлов CSkeletalNode.
    void VisitNodeTree(const aiScene& scene)
    {
        m_rootNode = std::make_unique<CSkeletalNode>();
        VisitNode(*scene.mRootNode, *m_rootNode);
        LoadAnimations(scene);
    }

    // Добавляет сетку треугольников в общий набор данных.
    void Add(const aiMesh& mesh)
    {
        if (mesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
        {
            throw std::runtime_error("Only triangle meshes are supported");
        }

        CSkeletalMesh3D mesh3d;
        mesh3d.m_materialIndex = mesh.mMaterialIndex;

        // Преобразуем данные о костях в форму, в которой вершина хранит данные
        //  о креплении к костям, а не кость хранит данные о прикрелённых
        //  вершинах.
        CollectPerVertexSkinning(mesh, mesh3d.m_bones);

        SetupBytesLayout(mesh, mesh3d.m_layout);

        CopyVertexes(mesh, mesh3d.m_layout);
        CopyIndexes(mesh);

        m_meshes.push_back(mesh3d);
    }

    std::vector<CSkeletalMesh3D> &&TakeMeshes()
    {
        return std::move(m_meshes);
    }

    std::vector<CModelAnimation> &&TakeAnimations()
    {
        return std::move(m_animations);
    }

    CSkeletalNodePtr &&TakeRoot()
    {
        return std::move(m_rootNode);
    }

    CGeometrySharedPtr MakeGeometry()const
    {
        auto pGeometry = std::make_shared<CGeometry>();
        pGeometry->Copy(m_geometry);
        return pGeometry;
    }

private:
    // Вычисляет ограничивающий параллелипипед сцены путём перебора всех вершин
    static CBoundingBox GetNodeBBox(const aiScene& scene, const aiNode& node)
    {
        vec3 lowerBound(+1e10f);
        vec3 upperBound(-1e10f);

        for (unsigned mi = 0; mi < node.mNumMeshes; ++mi)
        {
            const unsigned indexOnScene = node.mMeshes[mi];
            const aiMesh* mesh = scene.mMeshes[indexOnScene];
            for (unsigned vi = 0; vi < mesh->mNumVertices; ++vi)
            {
                const aiVector3D aiVertex = mesh->mVertices[vi];
                const vec3 vertex = glm::make_vec3(&aiVertex.x);
                lowerBound = glm::min(lowerBound, vertex);
                upperBound = glm::max(upperBound, vertex);
            }
        }

        CBoundingBox box(lowerBound, upperBound);

        for (unsigned ci = 0; ci < node.mNumChildren; ++ci) {
            const aiNode* pChildNode = node.mChildren[ci];
            CBoundingBox childBox = GetNodeBBox(scene, *pChildNode);
            box.Unite(childBox);
        }

        return box;
    }

    // Устанавливает смещения компонентов и размер вершины
    //  в байтах для подсети треугольников.
    void SetupBytesLayout(const aiMesh& mesh, SGeometryLayout &layout)
    {
        // TODO: реализуйте поддержку других примитивов (Lines, Points).
        layout.m_primitive = PrimitiveType::Triangles;

        layout.m_indexCount = size_t(TRI_VERTEX_COUNT * mesh.mNumFaces);
        layout.m_vertexCount = size_t(mesh.mNumVertices);
        layout.m_baseVertexOffset = GetBytesCount(m_geometry.m_vertexData);
        layout.m_baseIndexOffset = GetBytesCount(m_geometry.m_indicies);

        // Позиция есть у любой вершины
        layout.m_position3D = layout.m_vertexSize;
        layout.m_vertexSize += sizeof(aiVector3D);

        // Нормаль есть у любой вершины
        layout.m_normal = layout.m_vertexSize;
        layout.m_vertexSize += sizeof(aiVector3D);

        // Анимированная модель обязана иметь кости.
        layout.m_boneIndexes = layout.m_vertexSize;
        layout.m_vertexSize += sizeof(uint8_t) * BONES_PER_VERTEX;
        layout.m_boneWeights = layout.m_vertexSize;
        layout.m_vertexSize += sizeof(float) * BONES_PER_VERTEX;

        // Текстурные координаты UV - опциональный атрибут
        if (mesh.HasTextureCoords(0))
        {
            layout.m_texCoord2D = layout.m_vertexSize;
            layout.m_vertexSize += sizeof(aiVector2D);
        }
        // Тангенциальные и битангенциальные касательные сопутствуют друг другу.
        if (mesh.HasTangentsAndBitangents())
        {
            layout.m_tangent = layout.m_vertexSize;
            layout.m_vertexSize += sizeof(aiVector3D);
            layout.m_bitangent = layout.m_vertexSize;
            layout.m_vertexSize += sizeof(aiVector3D);
        }
    }

    void CopyVertexes(const aiMesh& mesh, SGeometryLayout &layout)
    {
        // Добавляем нужное число байт в массив,
        //  затем формируем указатель для начала записи данных.
        const size_t dataSize = layout.m_vertexCount * layout.m_vertexSize;
        uint8_t *pDest = AddItemsToWrite(m_geometry.m_vertexData, dataSize);

        for (unsigned i = 0, n = mesh.mNumVertices; i < n; i += 1)
        {
            // Копируем вершины (обязательный атрибут)
            std::memcpy(pDest + layout.m_position3D,
                        &mesh.mVertices[i].x,
                        sizeof(aiVector3D));

            // Копируем нормали (обязательный атрибут)
            std::memcpy(pDest + layout.m_normal,
                        &mesh.mNormals[i].x,
                        sizeof(aiVector3D));

            // Копируем текстурные координаты
            if (layout.m_texCoord2D != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_texCoord2D,
                            &mesh.mTextureCoords[0][i].x,
                            sizeof(aiVector2D));
            }

            // Копируем тангенциальные координаты
            if (layout.m_tangent != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_tangent,
                            &mesh.mTangents[i].x,
                            sizeof(aiVector3D));
            }

            // Копируем тангенциальные координаты
            if (layout.m_bitangent != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_bitangent,
                            &mesh.mBitangents[i].x,
                            sizeof(aiVector3D));
            }

            // Копируем веса костей
            if (layout.m_boneWeights != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_boneWeights,
                            &m_meshSkinning[i].weights,
                            sizeof(float) * BONES_PER_VERTEX);
            }

            // Копируем индексы костей
            if (layout.m_boneIndexes != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_boneIndexes,
                            &m_meshSkinning[i].indexes,
                            sizeof(uint8_t) * BONES_PER_VERTEX);
            }

            // Сдвигаем указатель на данные.
            pDest += layout.m_vertexSize;
        }
    }

    void CopyIndexes(const aiMesh& mesh)
    {
        // Добавляем нужное число элементов uint32_t в массив,
        //  затем формируем указатель для начала записи данных.
        const size_t dataSize = TRI_VERTEX_COUNT * mesh.mNumFaces;
        uint32_t *pDestData = AddItemsToWrite(m_geometry.m_indicies, dataSize);
        for (unsigned i = 0, n = mesh.mNumFaces; i < n; i += 1)
        {
            unsigned *indicies = mesh.mFaces[i].mIndices;
            std::memcpy(pDestData, indicies, sizeof(unsigned) * TRI_VERTEX_COUNT);
            pDestData += TRI_VERTEX_COUNT;
        }
    }

    // Рекурсивно вызываемая функция,
    //  собирающая информацию о скининке вершин сетки,
    //  и составляющее отображение номеров костей на кости.
    void CollectPerVertexSkinning(const aiMesh &mesh,
                                  std::vector<CSkeletalBone> &bones)
    {
        // Заполняем массивы пустыми данными нужном в количестве.
        m_meshSkinning.resize(mesh.mNumVertices, CVertexSkinning());
        std::fill(m_meshSkinning.begin(), m_meshSkinning.end(), CVertexSkinning());
        bones.resize(mesh.mNumBones);

        for (unsigned boneId = 0; boneId < mesh.mNumBones; ++boneId)
        {
            auto &bone = bones[boneId];
            const aiBone &srcBone = *mesh.mBones[boneId];
            const std::string boneName = srcBone.mName.C_Str();

            // Заполняем данные о кости.
            bone.m_pNode = m_nodeNameMapping.at(boneName);
            bone.m_boneOffset = CAssimpUtils::ConvertMat4(srcBone.mOffsetMatrix);

            for (unsigned wi = 0; wi < srcBone.mNumWeights; ++wi)
            {
                const aiVertexWeight &weight = srcBone.mWeights[wi];
                CVertexSkinning &skinning = m_meshSkinning.at(weight.mVertexId);
                skinning.AddWeight(boneId, weight.mWeight);
            }
        }
    }

    // Рекурсивно вызываемая функция,
    //  собирающая трансформации подсеток сцены.
    void VisitNode(const aiNode &srcNode, CSkeletalNode &node)
    {
        node.m_transform = CAssimpUtils::DecomposeTransform3D(srcNode.mTransformation);
        node.m_children.resize(srcNode.mNumChildren);
        node.m_name = srcNode.mName.C_Str();
        for (unsigned ci = 0; ci < srcNode.mNumChildren; ++ci)
        {
            node.m_children[ci] = std::make_unique<CSkeletalNode>();
            VisitNode(*srcNode.mChildren[ci], *node.m_children[ci]);
        }
        m_nodeNameMapping[node.m_name] = &node;
    }

    void LoadAnimations(const aiScene &scene)
    {
        m_animations.resize(scene.mNumAnimations);
        for (unsigned ai = 0; ai < scene.mNumAnimations; ++ai)
        {
            const aiAnimation &srcAnim = *scene.mAnimations[ai];
            CModelAnimation &anim = m_animations[ai];
            if (srcAnim.mNumMeshChannels != 0)
            {
                throw std::runtime_error("Mesh animation channels not implemented");
            }

            anim.m_name = srcAnim.mName.C_Str();
            anim.m_duration = srcAnim.mDuration;
            anim.m_ticksPerSecond = srcAnim.mTicksPerSecond;
            anim.m_channels.resize(srcAnim.mNumChannels);

            for (unsigned ci = 0; ci < srcAnim.mNumChannels; ++ci)
            {
                CopyNodeAnimation(*srcAnim.mChannels[ci], anim.m_channels[ci]);
            }
        }
    }

    void CopyNodeAnimation(const aiNodeAnim &srcAnim, CNodeAnimation &anim)
    {
        // Запоминаем узел, который будет анимирован.
        const std::string nodeName = srcAnim.mNodeName.C_Str();
        anim.m_pNode = m_nodeNameMapping.at(nodeName);

        // Копируем ключевые кадры для изменения размера.
        anim.m_scaleKeyframes.resize(srcAnim.mNumScalingKeys);
        for (unsigned ki = 0; ki < srcAnim.mNumScalingKeys; ++ki)
        {
            const auto &keyframe = srcAnim.mScalingKeys[ki];
            anim.m_scaleKeyframes[ki] = {
                keyframe.mTime, glm::make_vec3(&keyframe.mValue.x),
            };
        }

        // Копируем ключевые кадры для вращения.
        anim.m_rotationKeyframes.resize(srcAnim.mNumRotationKeys);
        for (unsigned ki = 0; ki < srcAnim.mNumRotationKeys; ++ki)
        {
            const auto &keyframe = srcAnim.mRotationKeys[ki];
            anim.m_rotationKeyframes[ki] = {
                keyframe.mTime, CAssimpUtils::ConvertQuat(keyframe.mValue),
            };
        }

        // Копируем ключевые кадры для перемещения.
        anim.m_positionKeyframes.resize(srcAnim.mNumPositionKeys);
        for (unsigned ki = 0; ki < srcAnim.mNumPositionKeys; ++ki)
        {
            const auto &keyframe = srcAnim.mPositionKeys[ki];
            anim.m_positionKeyframes[ki] = {
                keyframe.mTime, glm::make_vec3(&keyframe.mValue.x),
            };
        }
    }

    std::vector<CSkeletalMesh3D> m_meshes;
    std::vector<CModelAnimation> m_animations;
    SGeometryData<uint8_t, uint32_t> m_geometry;
    CSkeletalNodePtr m_rootNode;
    std::unordered_map<std::string, CSkeletalNode*> m_nodeNameMapping;
    std::vector<CVertexSkinning> m_meshSkinning;
};

// Проверяет целостность данных сетки треугольников.
// Выбрасывает std::runtime_error, если в данные закралась ошибка.
void VerifyModel3D(CSkeletalModel3D &model)
{
    for (const CSkeletalMesh3D &mesh : model.m_meshes)
    {
        if (mesh.m_materialIndex >= model.m_materials.size())
        {
            const std::string indexStr = std::to_string(mesh.m_materialIndex);
            throw std::runtime_error("Material #" + indexStr + " does not exist");
        }
    }
}
}


CSkeletalModelLoader::CSkeletalModelLoader(CAssetLoader &assetLoader)
    : m_assetLoader(assetLoader)
{
}

CSkeletalModel3DPtr CSkeletalModelLoader::Load(const boost::filesystem::path &path)
{
    const boost::filesystem::path abspath = m_assetLoader.GetResourceAbspath(path);

    Assimp::Importer importer;

    // Для загружаемых моделей ограничим число костей, влияющих на вершину.
    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, BONES_PER_VERTEX);

    const aiScene &scene = CAssimpUtils::OpenScene(abspath, importer);

    // В отладочном режиме выводим информацию о модели.
#if !defined(NDEBUG)
    CAssimpUtils::DumpSceneInfo(scene);
#endif

    CMeshAccumulator accumulator;
    accumulator.CollectBoundingBox(scene);
    accumulator.VisitNodeTree(scene);
    for (unsigned mi = 0; mi < scene.mNumMeshes; ++mi)
    {
        accumulator.Add(*(scene.mMeshes[mi]));
    }

    auto pModel = std::make_shared<CSkeletalModel3D>();
    pModel->m_meshes = accumulator.TakeMeshes();
    pModel->m_animations = accumulator.TakeAnimations();
    pModel->m_rootNode = accumulator.TakeRoot();
    pModel->m_pGeometry = accumulator.MakeGeometry();
    CAssimpUtils::LoadMaterials(abspath.parent_path(), m_assetLoader,
                                scene, pModel->m_materials);
    VerifyModel3D(*pModel);

    return pModel;
}
