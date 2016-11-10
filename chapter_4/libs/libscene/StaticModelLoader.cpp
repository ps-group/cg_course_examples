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

class CMaterialReader
{
public:
    CMaterialReader(const aiMaterial& srcMat, const path &resourceDir,
                    CAssetLoader &assetLoader)
        : m_srcMat(srcMat)
        , m_resourceDir(resourceDir)
        , m_assetLoader(assetLoader)
    {
    }

    // Если в материале задан указанный цвет,
    //  возвращает его нормализованное к диапазону [0..1] значение в RGBA.
    // Иначе возвращает defaultValue
    vec4 GetColor(const char *key, unsigned type, unsigned index)
    {
        aiColor3D color(0, 0, 0);
        if (AI_SUCCESS == m_srcMat.Get(key, type, index, color))
        {
            return glm::clamp(vec4(color.r, color.g, color.b, 1), vec4(0), vec4(1));
        }
        return vec4(0);
    }

    float GetFloat(const char *key, unsigned type, unsigned index)
    {
        float value = 0;
        if (AI_SUCCESS == m_srcMat.Get(key, type, index, value))
        {
            return value;
        }
        return 0.f;
    }

    unsigned GetUnsigned(const char *key, unsigned type, unsigned index)
    {
        unsigned value = 0;
        if (AI_SUCCESS == m_srcMat.Get(key, type, index, value))
        {
            return value;
        }
        return 0;
    }

    // Возвращает указатель на текстуру, расположенную в директории модели.
    CTexture2DSharedPtr GetTexture(const char *key, unsigned type, unsigned index)
    {
        aiString filename;
        if (AI_SUCCESS == m_srcMat.Get(key, type, index, filename))
        {
            path abspath = m_resourceDir / filename.data;
            return m_assetLoader.LoadTexture(abspath);
        }
        return nullptr;
    }

private:
    const aiMaterial& m_srcMat;
    path m_resourceDir;
    CAssetLoader &m_assetLoader;
};

// Накапливает информацию о сетках треугольников и геометрии.
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

    // Обходит все узлы сцены и запоминает суммарную трансформацию
    //  для каждой подсетки.
    void CollectTransforms(const aiScene& scene)
    {
        CollectTransformsImpl(*scene.mRootNode, glm::mat4());
    }

    // Добавляет сетку треугольников в общий набор данных.
    void Add(const aiMesh& mesh)
    {
        if (mesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
        {
            throw std::runtime_error("Only triangle meshes are supported");
        }
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
        CopyIndexes(mesh);

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
                upperBound = glm::min(upperBound, vertex);
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
    //  собирающая трансформации подсеток сцены.
    void CollectTransformsImpl(const aiNode &node, const glm::mat4 &parentTransform)
    {
        const auto localMat4 = glm::make_mat4(&node.mTransformation.a1);

        // В OpenGL матрицы по умолчанию принимаются в виде
        //  "столбец за столбцом", а не "строка за строкой",
        //  поэтому мы транспонируем матрицу локального преобразования.
        const auto globalMat4 = parentTransform * glm::transpose(localMat4);

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
            CollectTransformsImpl(*node.mChildren[ci], globalMat4);
        }
    }

    std::vector<CStaticMesh3D> m_meshes;
    SGeometryData<uint8_t, uint32_t> m_geometry;
    std::unordered_map<unsigned, glm::mat4> m_meshTransforms;
};

bool CanBePhongShaded(unsigned shadingMode)
{
    switch (shadingMode)
    {
    case 0:
    case aiShadingMode_Phong:
    case aiShadingMode_Blinn:
    case aiShadingMode_Gouraud:
    case aiShadingMode_Flat:
        return true;
    default:
        return false;
    }
}

void LoadMaterials(const path &resourceDir, CAssetLoader &assetLoader,
                   const aiScene &scene, std::vector<SPhongMaterial> &materials)
{
    const float DEFAULT_SHININESS = 30;

    materials.resize(scene.mNumMaterials);
    for (unsigned mi = 0; mi < scene.mNumMaterials; ++mi)
    {
        CMaterialReader reader(*(scene.mMaterials[mi]), resourceDir, assetLoader);
        SPhongMaterial &material = materials[mi];

        // TODO: реализовать новые модели освещения
        //       например, aiShadingMode_CookTorrance, aiShadingMode_Toon.
        const unsigned shadingMode = reader.GetUnsigned(AI_MATKEY_SHADING_MODEL);
        if (!CanBePhongShaded(shadingMode))
        {
            throw std::runtime_error("Given shading model was not implemented");
        }

        material.shininess = reader.GetFloat(AI_MATKEY_SHININESS);
        if (material.shininess < 1.f)
        {
            material.shininess = DEFAULT_SHININESS;
        }
        material.pDiffuse = reader.GetTexture(AI_MATKEY_TEXTURE_DIFFUSE(0));
        if (!material.pDiffuse)
        {
            material.diffuseColor = reader.GetColor(AI_MATKEY_COLOR_DIFFUSE);
        }
        material.pSpecular = reader.GetTexture(AI_MATKEY_TEXTURE_SPECULAR(0));
        if (!material.pSpecular)
        {
            material.specularColor = reader.GetColor(AI_MATKEY_COLOR_SPECULAR);
        }
        material.pEmissive = reader.GetTexture(AI_MATKEY_TEXTURE_EMISSIVE(0));
        if (!material.pEmissive)
        {
            material.emissiveColor = reader.GetColor(AI_MATKEY_COLOR_EMISSIVE);
        }
    }
}

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
    accumulator.CollectTransforms(scene);
    for (unsigned mi = 0; mi < scene.mNumMeshes; ++mi)
    {
        accumulator.Add(*(scene.mMeshes[mi]));
    }

    auto pModel = std::make_shared<CStaticModel3D>();
    pModel->m_meshes = accumulator.TakeMeshes();
    pModel->m_pGeometry = accumulator.MakeGeometry();
    LoadMaterials(abspath.parent_path(), m_assetLoader, scene, pModel->m_materials);
    VerifyModel3D(*pModel);

    return pModel;
}
