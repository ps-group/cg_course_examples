#include "libchapter4_private.h"
#include "ModelLoader.h"
#include "AssetLoader.h"
#include <sstream>

// Для импорта используем библиотеку Assimp
//  см. http://assimp.sourceforge.net/
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

using boost::filesystem::path;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace
{

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

class CMeshAccumulator
{
public:
    static const size_t RESERVED_SIZE = 4 * 1024;
    static const unsigned TRI_VERTEX_COUNT = 3;

    CMeshAccumulator(SComplexMeshData &data)
        : m_data(data)
    {
        m_data.m_vertexData.reserve(RESERVED_SIZE);
        m_data.m_indicies.reserve(RESERVED_SIZE);
    }

    // Обходит все узлы сцены и запоминает суммарную трансформацию
    //  для каждой подсетки.
    void CollectTransforms(const aiNode &pNode)
    {
        CollectTransformsImpl(pNode, glm::mat4());
    }

    // Добавляет сетку треугольников в общий набор данных.
    void Add(const aiMesh& mesh)
    {
        if (mesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
        {
            throw std::runtime_error("Only triangle meshes are supported");
        }
        const unsigned submeshNo = unsigned(m_data.m_submeshes.size());

        SSubMesh submesh;
        try
        {
            submesh.m_transform = m_meshTransforms.at(submeshNo);
        }
        catch (const std::out_of_range &)
        {
            throw std::out_of_range("Submesh #" + std::to_string(submeshNo)
                                    + " has no transform");
        }
        submesh.m_vertexRange = {
            0u,
            mesh.mNumVertices - 1u,
        };
        submesh.m_indexRange = {
            unsigned(m_data.m_indicies.size()),
            unsigned(m_data.m_indicies.size()) + TRI_VERTEX_COUNT * mesh.mNumFaces
        };

        submesh.m_materialIndex = mesh.mMaterialIndex;
        SetupBytesLayout(mesh, submesh);
        CopyVertexes(mesh, submesh);
        CopyIndexes(mesh);

        m_data.m_submeshes.push_back(submesh);
    }

private:
    void CopyVertexes(const aiMesh& mesh, const SSubMesh &submesh)
    {
        // Добавляем нужное число байт в массив,
        //  затем формируем указатель для начала записи данных.
        const size_t oldSize = m_data.m_vertexData.size();
        const size_t submeshSize = submesh.m_stride * mesh.mNumVertices;
        m_data.m_vertexData.resize(oldSize + submeshSize);

        uint8_t *pDest = m_data.m_vertexData.data() + oldSize;
        for (unsigned i = 0, n = mesh.mNumVertices; i < n; i += 1)
        {
            // Копируем нормали и вершины
            std::memcpy(pDest + submesh.m_positionOffset,
                        &mesh.mVertices[i].x, sizeof(aiVector3D));
            std::memcpy(pDest + submesh.m_normalsOffset,
                        &mesh.mNormals[i].x, sizeof(aiVector3D));

            // Копируем текстурные координаты
            if (submesh.m_textureOffset != -1)
            {
                std::memcpy(pDest + submesh.m_textureOffset,
                            &mesh.mTextureCoords[0][i].x, sizeof(aiVector2D));
            }

            // Копируем тангенциальные координаты
            if (submesh.m_tangentsOffset != -1)
            {
                std::memcpy(pDest + submesh.m_tangentsOffset,
                            &mesh.mTangents[i].x, sizeof(aiVector3D));
            }

            // Сдвигаем указатель на данные.
            pDest += submesh.m_stride;
        }
    }

    // Устанавливает смещения компонентов и размер вершины
    //  в байтах для подсети треугольников.
    void SetupBytesLayout(const aiMesh& mesh, SSubMesh &submesh)
    {
        submesh.m_baseOffset = unsigned(m_data.m_vertexData.size());
        submesh.m_positionOffset = 0;
        submesh.m_normalsOffset = sizeof(aiVector3D);
        unsigned vertexSize = 2 * sizeof(aiVector3D); // Нормали + вершины
        if (mesh.HasTextureCoords(0))
        {
            submesh.m_textureOffset = int(vertexSize);
            vertexSize += sizeof(aiVector2D); // Текстурные координаты UV
        }
        if (mesh.HasTangentsAndBitangents())
        {
            submesh.m_tangentsOffset = int(vertexSize);
            vertexSize += sizeof(aiVector3D); // Тангенциальные касательные
        }
        submesh.m_stride = vertexSize;
    }

    void CopyIndexes(const aiMesh& mesh)
    {
        // Добавляем нужное число элементов uint32_t в массив,
        //  затем формируем указатель для начала записи данных.
        const size_t oldSize = m_data.m_indicies.size();
        const size_t submeshSize = TRI_VERTEX_COUNT * mesh.mNumFaces;
        m_data.m_indicies.resize(oldSize + submeshSize);
        uint32_t *pDestData = m_data.m_indicies.data() + oldSize;

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

    SComplexMeshData &m_data;
    std::unordered_map<unsigned, glm::mat4> m_meshTransforms;
};

const aiScene *OpenScene(const path &path,
                         Assimp::Importer &importer)
{
    // Памятью для хранения сцены владеет importer
    const unsigned importFlags = aiProcessPreset_TargetRealtime_Fast;
    const aiScene *scene = importer.ReadFile(path.generic_string().c_str(), importFlags);
    if (scene == nullptr)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    return scene;
}

// Вычисляет ограничивающий параллелипипед сцены путём перебора всех вершин
CBoundingBox GetNodeBoundingBox(const aiScene& scene, const aiNode& node)
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
        CBoundingBox childBox = GetNodeBoundingBox(scene, *pChildNode);
        box.Unite(childBox);
    }

    return box;
}

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
                   const aiScene &scene, std::vector<SMaterial> &materials)
{
    materials.resize(scene.mNumMaterials);
    for (unsigned mi = 0; mi < scene.mNumMaterials; ++mi)
    {
        CMaterialReader reader(*(scene.mMaterials[mi]), resourceDir, assetLoader);
        SMaterial &material = materials[mi];

        // TODO: реализовать новые модели освещения
        //       например, aiShadingMode_CookTorrance, aiShadingMode_Toon.
        const unsigned shadingMode = reader.GetUnsigned(AI_MATKEY_SHADING_MODEL);
        if (!CanBePhongShaded(shadingMode))
        {
            throw std::runtime_error("Given shading model was not implemented");
        }

        material.m_shininess = reader.GetFloat(AI_MATKEY_SHININESS);
        material.m_diffuseColor = reader.GetColor(AI_MATKEY_COLOR_DIFFUSE);
        material.m_specularColor = reader.GetColor(AI_MATKEY_COLOR_SPECULAR);
        material.m_emissiveColor = reader.GetColor(AI_MATKEY_COLOR_EMISSIVE);
        material.m_pDiffuse = reader.GetTexture(AI_MATKEY_TEXTURE_DIFFUSE(0));
        material.m_pSpecular = reader.GetTexture(AI_MATKEY_TEXTURE_SPECULAR(0));
        material.m_pEmissive = reader.GetTexture(AI_MATKEY_TEXTURE_EMISSIVE(0));
    }
}

void LoadMeshes(const aiScene &scene, SComplexMeshData &data)
{
    CMeshAccumulator accumulator(data);
    accumulator.CollectTransforms(*scene.mRootNode);
    for (unsigned mi = 0; mi < scene.mNumMeshes; ++mi)
    {
        accumulator.Add(*scene.mMeshes[mi]);
    }
}

// Проверяет целостность данных сетки треугольников.
// Выбрасывает std::runtime_error, если в данные закралась ошибка.
void VerifyMeshData(SComplexMeshData &data)
{
    for (const SSubMesh &mesh : data.m_submeshes)
    {
        if (mesh.m_materialIndex >= data.m_materials.size())
        {
            const std::string indexStr = std::to_string(mesh.m_materialIndex);
            throw std::runtime_error("Material #" + indexStr + " does not exist");
        }
    }
}
}


CModelLoader::CModelLoader(CAssetLoader &assetLoader)
    : m_assetLoader(assetLoader)
{
}

void CModelLoader::Load(const boost::filesystem::path &path, SComplexMeshData &data)
{
    const boost::filesystem::path abspath = m_assetLoader.GetResourceAbspath(path);

    data.m_materials.clear();
    data.m_submeshes.clear();
    data.m_indicies.clear();
    data.m_vertexData.clear();

    Assimp::Importer importer;
    const aiScene *pScene = OpenScene(abspath, importer);
    data.m_bbox = GetNodeBoundingBox(*pScene, *pScene->mRootNode);
    LoadMaterials(abspath.parent_path(), m_assetLoader, *pScene, data.m_materials);
    LoadMeshes(*pScene, data);

    VerifyMeshData(data);
}

// Обходит дерево узлов (aiNode) сцены (aiScene),
//  печатает имя каждого узла.
class CNodeTreeDumper
{
public:
    void Inspect(aiNode *pNode)
    {
        if (!pNode)
        {
            return;
        }
        m_indentLevel += 1;
        PrintNodeInfo(pNode->mName.C_Str(), pNode->mTransformation);
        for (unsigned i = 0, n = pNode->mNumChildren; i < n; ++i)
        {
            Inspect(pNode->mChildren[i]);
        }
        m_indentLevel -= 1;
    }

private:
    void PrintNodeInfo(const std::string &name, const aiMatrix4x4 &transform)
    {
        const unsigned SIZE = 4;
        const std::string indent(2 * m_indentLevel, ' ');

        std::cerr << indent << "Node " << name << std::endl;
        for (unsigned row = 0; row < SIZE; ++row)
        {
            std::cerr << indent << "[ ";
            for (unsigned column = 0; column < SIZE; ++column)
            {
                const float value = transform[row][column];
                std::cerr << value << " ";
            }
            std::cerr << "]" << std::endl;
        }
    }

    unsigned m_indentLevel = 0;
};

class CMaterialDumper
{
public:
    void PrintValue(const std::string &key, const std::string &value)
    {
        std::cerr << "  property " << key << " = " << value << std::endl;
    }

    void PrintMaterialTitle(unsigned materialNo)
    {
        std::cerr << "-- material #" << materialNo << " --" << std::endl;
    }

    void Inspect(unsigned materialNo, const aiMaterial &mat)
    {
        PrintMaterialTitle(materialNo);
        for (unsigned pi = 0; pi < mat.mNumProperties; ++pi)
        {
            const aiMaterialProperty &prop = *mat.mProperties[pi];

            switch (prop.mType)
            {
            case aiPTI_Float:
                DumpFloatProperty(mat, prop);
                break;
            case aiPTI_String:
                DumpStringProperty(mat, prop);
                break;
            case aiPTI_Integer:
                DumpIntProperty(mat, prop);
            case aiPTI_Buffer:
                DumpBufferProperty(mat, prop);
                break;
            default:
                break;
            }

        }
    }

private:
    void DumpFloatProperty(const aiMaterial &mat, const aiMaterialProperty &prop)
    {
        float buffer[100];
        unsigned count = 100;
        if (AI_SUCCESS == mat.Get(prop.mKey.C_Str(), prop.mSemantic, prop.mIndex, buffer, &count))
        {
            PrintValue(prop.mKey.C_Str(), ArrayToString(buffer, count));
        }
        else
        {
            PrintValue(prop.mKey.C_Str(), "<float read failed>");
        }
    }

    void DumpStringProperty(const aiMaterial &mat, const aiMaterialProperty &prop)
    {
        aiString value;
        if (AI_SUCCESS == mat.Get(prop.mKey.C_Str(), prop.mSemantic, prop.mIndex, value))
        {
            PrintValue(prop.mKey.C_Str(), std::string("'") + value.C_Str() + "'");
        }
        else
        {
            PrintValue(prop.mKey.C_Str(), "<string read failed>");
        }
    }

    void DumpIntProperty(const aiMaterial &mat, const aiMaterialProperty &prop)
    {
        int buffer[100];
        unsigned count = 100;
        if (AI_SUCCESS == mat.Get(prop.mKey.C_Str(), prop.mSemantic, prop.mIndex, buffer, &count))
        {
            PrintValue(prop.mKey.C_Str(), ArrayToString(buffer, count));
        }
        else
        {
            PrintValue(prop.mKey.C_Str(), "<int read failed>");
        }
    }

    void DumpBufferProperty(const aiMaterial &mat, const aiMaterialProperty &prop)
    {
        PrintValue(prop.mKey.C_Str(), "<some data buffer>");
    }

    template <class T>
    std::string ArrayToString(T values[], unsigned count)
    {
        std::stringstream stream;
        for (unsigned i = 0; i < count; ++i)
        {
            stream << values[i];
            if (i + 1 != count)
            {
                stream << ", ";
            }
        }
        return stream.str();
    }
};

void CModelLoader::DumpInfo(const boost::filesystem::path &path)
{
    const boost::filesystem::path abspath = m_assetLoader.GetResourceAbspath(path);
    Assimp::Importer importer;
    const aiScene *pScene = OpenScene(abspath, importer);

    std::cerr << "3D Model loaded"
              << ", " << pScene->mNumMeshes << " meshes"
              << ", " << pScene->mNumMaterials << " materials"
              << ", " << pScene->mNumAnimations << " animations"
              << ", " << pScene->mNumLights << " lights"
              << std::endl;

    CNodeTreeDumper inspector;
    inspector.Inspect(pScene->mRootNode);

    CMaterialDumper materialInspector;
    for (unsigned mi = 0; mi < pScene->mNumMaterials; ++mi)
    {
        materialInspector.Inspect(mi, *(pScene->mMaterials[mi]));
    }
}
