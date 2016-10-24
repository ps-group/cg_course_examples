#include "libchapter4_private.h"
#include "ModelLoader.h"

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
                    const CModelLoader::LoadTextureFn &loadTexture)
        : m_srcMat(srcMat)
        , m_resourceDir(resourceDir)
        , m_loadTexture(loadTexture)
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
            return m_loadTexture(abspath);
        }
        return nullptr;
    }

private:
    const aiMaterial& m_srcMat;
    path m_resourceDir;
    CModelLoader::LoadTextureFn m_loadTexture;
};

class CMeshAccumulator
{
public:
    static const size_t RESERVED_SIZE = 4 * 1024;

    CMeshAccumulator(SComplexMeshData &data)
        : m_data(data)
    {
        m_data.m_vertexData.reserve(RESERVED_SIZE);
        m_data.m_indicies.reserve(RESERVED_SIZE);
    }

    void Add(const aiMesh& mesh)
    {
        if (mesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
        {
            throw std::runtime_error("Only triangle meshes are supported");
        }

        SSubMesh submesh;
        submesh.m_vertexRange = {
            m_lastVertexIndex + 1,
            m_lastVertexIndex + mesh.mNumVertices
        };
        submesh.m_indexRange = {
            unsigned(m_data.m_indicies.size()),
            unsigned(m_data.m_indicies.size()) + mesh.mNumFaces
        };
        m_lastVertexIndex += mesh.mNumVertices;

        submesh.m_materialIndex = mesh.mMaterialIndex;
        submesh.m_hasTexCoord = mesh.HasTextureCoords(0);
        submesh.m_hasTangentSpace = mesh.HasTangentsAndBitangents();

        m_data.m_submeshes.push_back(submesh);
        CopyVertexes(mesh, submesh);
        CopyIndexes(mesh);
    }

private:
    void CopyVertexes(const aiMesh& mesh, const SSubMesh &submesh)
    {
        const size_t numComponents = GetVertexComponentsCount(submesh);

        // Добавляем нужное число элементов float в массив,
        //  затем формируем указатель для начала записи данных.
        const size_t oldSize = m_data.m_vertexData.size();
        m_data.m_vertexData.resize(oldSize + numComponents * mesh.mNumVertices);
        float *pDestData = m_data.m_vertexData.data() + oldSize;

        auto addData = [&](const float *srcData, size_t count) {
            std::memcpy(pDestData, srcData, sizeof(float) * count);
            pDestData += count;
        };

        for (unsigned i = 0, n = mesh.mNumVertices; i < n; i += 1)
        {
            // Копируем нормали и вершины
            addData(&mesh.mVertices[i].x, 3);
            addData(&mesh.mNormals[i].x, 3);

            // Копируем текстурные координаты
            if (submesh.m_hasTexCoord)
            {
                addData(&mesh.mNormals[i].x, 2);
            }

            // Копируем тангенциальные координаты
            if (submesh.m_hasTangentSpace)
            {
                addData(&mesh.mTangents[i].x, 3);
            }
        }
    }

    size_t GetVertexComponentsCount(const SSubMesh &submesh)
    {
        size_t vertexSize = 6; // Нормали + вершины
        if (submesh.m_hasTexCoord)
        {
            vertexSize += 2; // Текстурные координаты UV
        }
        if (submesh.m_hasTangentSpace)
        {
            vertexSize += 3; // Тангенциальные касательные
        }
        return vertexSize;
    }

    void CopyIndexes(const aiMesh& mesh)
    {
        const unsigned triangleVertexCount = 3;
        const unsigned indexOffset = unsigned(m_data.m_indicies.size());

        // Добавляем нужное число элементов uint32_t в массив,
        //  затем формируем указатель для начала записи данных.
        const size_t oldSize = m_data.m_indicies.size();
        m_data.m_indicies.resize(oldSize + triangleVertexCount * mesh.mNumFaces);
        uint32_t *pDestData = m_data.m_indicies.data() + oldSize;

        for (unsigned i = 0, n = mesh.mNumFaces; i < n; i += 1)
        {
            unsigned *indicies = mesh.mFaces[i].mIndices;
            pDestData[0] = indexOffset + indicies[0];
            pDestData[1] = indexOffset + indicies[1];
            pDestData[2] = indexOffset + indicies[2];
            pDestData += triangleVertexCount;
        }
    }

    SComplexMeshData &m_data;
    unsigned m_lastVertexIndex = 0;
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


void LoadMaterials(const path &resourceDir, const CModelLoader::LoadTextureFn &loadTexture,
                   const aiScene &scene, std::vector<SMaterial> &materials)
{
    materials.resize(scene.mNumMaterials);
    for (unsigned mi = 0; mi < scene.mNumMaterials; ++mi)
    {
        CMaterialReader reader(*(scene.mMaterials[mi]), resourceDir, loadTexture);
        SMaterial &material = materials[mi];

        // TODO: реализовать новые модели освещения
        //       например, aiShadingMode_CookTorrance.
        const aiShadingMode shadingMode = static_cast<aiShadingMode>(
                    reader.GetUnsigned(AI_MATKEY_SHADING_MODEL));
        if (shadingMode != aiShadingMode_Phong
            && shadingMode != aiShadingMode_Blinn)
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


CModelLoader::CModelLoader(const LoadTextureFn &loadTextureFn)
    : m_loadTextureFn(loadTextureFn)
{
}

void CModelLoader::Load(const boost::filesystem::path &path, SComplexMeshData &data)
{
    data.m_materials.clear();
    data.m_submeshes.clear();
    data.m_indicies.clear();
    data.m_vertexData.clear();

    Assimp::Importer importer;
    const aiScene *pScene = OpenScene(path, importer);
    data.m_bbox = GetNodeBoundingBox(*pScene, *pScene->mRootNode);
    LoadMaterials(path.parent_path(), m_loadTextureFn, *pScene, data.m_materials);
    LoadMeshes(*pScene, data);

    VerifyMeshData(data);
}
