#include "StaticModelLoader.h"
#include "AssetLoader.h"
#include "AssimpUtils.h"

using boost::filesystem::path;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace
{
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

PrimitiveType MapPrimitiveType(unsigned aiPrimitiveTypeValue)
{
    switch (aiPrimitiveTypeValue)
    {
    case aiPrimitiveType_POINT:
        return PrimitiveType::Points;
    case aiPrimitiveType_LINE:
        return PrimitiveType::Lines;
    case aiPrimitiveType_TRIANGLE:
        return PrimitiveType::Triangles;
    }
    throw std::runtime_error("Unsupported assimp primitive type "
                             + std::to_string(aiPrimitiveTypeValue));
}

unsigned GetPrimitiveVertexCount(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::Points:
        return 1;
    case PrimitiveType::Lines:
        return 2;
    case PrimitiveType::Triangles:
        return 3;
    default:
        break;
    }
    throw std::runtime_error("Unexpected internal primitive type "
                             + std::to_string(static_cast<unsigned>(type)));
}

// Накапливает информацию о сетках треугольников и геометрии.
class CMeshAccumulator
{
public:
    static const size_t RESERVED_SIZE = 4 * 1024;

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

    // Обходит все узлы сцены и запоминает суммарную трансформацию
    //  для каждой подсетки.
    void VisitNodeTree(const aiScene& scene)
    {
        VisitNode(*scene.mRootNode, glm::mat4());
    }

    // Добавляет сетку треугольников в общий набор данных.
    void Add(const aiMesh& mesh)
    {
        const unsigned meshNo = unsigned(m_meshes.size());
        CStaticMesh3D mesh3d;
        mesh3d.m_materialIndex = mesh.mMaterialIndex;
        try
        {
            mesh3d.m_local = m_meshTransforms.at(meshNo);
        }
        catch (const std::out_of_range &)
        {
            throw std::out_of_range("Submesh #" + std::to_string(meshNo)
                                    + " has no transform");
        }
        SetupBytesLayout(mesh, mesh3d.m_layout);
        CopyVertexes(mesh, mesh3d.m_layout);
        CopyIndexes(mesh, mesh3d.m_layout);

        m_meshes.push_back(mesh3d);
    }

    std::vector<CStaticMesh3D> &&TakeMeshes()
    {
        return std::move(m_meshes);
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
        const PrimitiveType primitive = MapPrimitiveType(mesh.mPrimitiveTypes);
        const unsigned vertexPerPrimitive = GetPrimitiveVertexCount(primitive);

        layout.m_primitive = primitive;
        layout.m_indexCount = size_t(vertexPerPrimitive * mesh.mNumFaces);
        layout.m_vertexCount = size_t(mesh.mNumVertices);
        layout.m_baseVertexOffset = GetBytesCount(m_geometry.m_vertexData);
        layout.m_baseIndexOffset = GetBytesCount(m_geometry.m_indicies);

        // Позиция есть у любой вершины
        layout.m_position3D = layout.m_vertexSize;
        layout.m_vertexSize += sizeof(aiVector3D);

        // Нормаль есть у любой вершины
        layout.m_normal = layout.m_vertexSize;
        layout.m_vertexSize += sizeof(aiVector3D);

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
            // Копируем нормали и вершины
            std::memcpy(pDest + layout.m_position3D,
                        &mesh.mVertices[i].x, sizeof(aiVector3D));
            std::memcpy(pDest + layout.m_normal,
                        &mesh.mNormals[i].x, sizeof(aiVector3D));

            // Копируем текстурные координаты
            if (layout.m_texCoord2D != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_texCoord2D,
                            &mesh.mTextureCoords[0][i].x, sizeof(aiVector2D));
            }

            // Копируем тангенциальные координаты
            if (layout.m_tangent != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_tangent,
                            &mesh.mTangents[i].x, sizeof(aiVector3D));
            }

            // Копируем тангенциальные координаты
            if (layout.m_bitangent != SGeometryLayout::UNSET)
            {
                std::memcpy(pDest + layout.m_bitangent,
                            &mesh.mBitangents[i].x, sizeof(aiVector3D));
            }

            // Сдвигаем указатель на данные.
            pDest += layout.m_vertexSize;
        }
    }

    void CopyIndexes(const aiMesh& mesh, SGeometryLayout &layout)
    {
        // Добавляем нужное число элементов uint32_t в массив,
        //  затем формируем указатель для начала записи данных.
        const unsigned vertexPerPrimitive = GetPrimitiveVertexCount(layout.m_primitive);
        const size_t dataSize = vertexPerPrimitive * mesh.mNumFaces;
        uint32_t *pDestData = AddItemsToWrite(m_geometry.m_indicies, dataSize);

        // Копируем данные о всех гранях в единый буфер байт.
        for (unsigned i = 0, n = mesh.mNumFaces; i < n; i += 1)
        {
            unsigned *indicies = mesh.mFaces[i].mIndices;
            std::memcpy(pDestData, indicies, sizeof(unsigned) * vertexPerPrimitive);
            pDestData += vertexPerPrimitive;
        }
    }

    // Рекурсивно вызываемая функция,
    //  собирающая трансформации подсеток сцены.
    void VisitNode(const aiNode &node, const glm::mat4 &parentTransform)
    {
        const glm::mat4 globalMat4 = parentTransform
                * CAssimpUtils::ConvertMat4(node.mTransformation);

        for (unsigned mi = 0; mi < node.mNumMeshes; ++mi)
        {
            const unsigned meshNo = node.mMeshes[mi];
            if (m_meshTransforms.count(meshNo))
            {
                // В данном загрузчике модели не будет обработана ситуация,
                //  когда несколько узлов совместно используют одну сетку.
                throw std::runtime_error("Mesh #" + std::to_string(meshNo)
                                         + " used twice in node tree");
            }
            m_meshTransforms[meshNo] = globalMat4;
        }
        for (unsigned ci = 0; ci < node.mNumChildren; ++ci)
        {
            VisitNode(*node.mChildren[ci], globalMat4);
        }
    }

    std::vector<CStaticMesh3D> m_meshes;
    SGeometryData<uint8_t, uint32_t> m_geometry;
    std::unordered_map<unsigned, glm::mat4> m_meshTransforms;
};

// Проверяет целостность данных сетки треугольников.
// Выбрасывает std::runtime_error, если в данные закралась ошибка.
void VerifyModel3D(CStaticModel3D &model)
{
    for (const CStaticMesh3D &mesh : model.m_meshes)
    {
        if (mesh.m_materialIndex >= model.m_materials.size())
        {
            const std::string indexStr = std::to_string(mesh.m_materialIndex);
            throw std::runtime_error("Material #" + indexStr + " does not exist");
        }
    }
}
}


CStaticModelLoader::CStaticModelLoader(CAssetLoader &assetLoader)
    : m_assetLoader(assetLoader)
{
}

CStaticModel3DPtr CStaticModelLoader::Load(const boost::filesystem::path &path)
{
    const boost::filesystem::path abspath = m_assetLoader.GetResourceAbspath(path);

    Assimp::Importer importer;
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

    auto pModel = std::make_shared<CStaticModel3D>();
    pModel->m_meshes = accumulator.TakeMeshes();
    pModel->m_pGeometry = accumulator.MakeGeometry();
    CAssimpUtils::LoadMaterials(abspath.parent_path(), m_assetLoader,
                                scene, pModel->m_materials);
    VerifyModel3D(*pModel);

    return pModel;
}
