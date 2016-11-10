#include "AssimpUtils.h"
#include "includes/glm-common.hpp"
#include "Geometry.h"
#include "AssetLoader.h"
#include <sstream>
#include <iostream>

using boost::filesystem::path;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace
{
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

void PrintAiMatrix4(const std::string &indent, const aiMatrix4x4 &transform)
{
    const unsigned SIZE = 4;
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
        const std::string indent(2 * m_indentLevel, ' ');
        std::cerr << indent << "Node " << name << std::endl;
        PrintAiMatrix4(indent, transform);
    }

    unsigned m_indentLevel = 0;
};

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
        (void)mat;
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
}

const aiScene &CAssimpUtils::OpenScene(const boost::filesystem::path &path, Assimp::Importer &importer)
{
    const unsigned importFlags = aiProcessPreset_TargetRealtime_Fast;
    const aiScene *pScene = importer.ReadFile(path.generic_string().c_str(), importFlags);
    if (pScene == nullptr)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    return *pScene;
}

void CAssimpUtils::DumpSceneInfo(const aiScene &scene)
{

    std::cerr << "3D Model loaded"
              << ", " << scene.mNumMeshes << " meshes"
              << ", " << scene.mNumMaterials << " materials"
              << ", " << scene.mNumAnimations << " animations"
              << ", " << scene.mNumLights << " lights"
              << std::endl;

    CNodeTreeDumper inspector;
    inspector.Inspect(scene.mRootNode);

    CMaterialDumper materialInspector;
    for (unsigned mi = 0; mi < scene.mNumMaterials; ++mi)
    {
        materialInspector.Inspect(mi, *(scene.mMaterials[mi]));
    }

    for (unsigned mi = 0; mi < scene.mNumMeshes; ++mi)
    {
        const auto *pMesh = scene.mMeshes[mi];
        if (pMesh->HasBones())
        {
            std::cerr << "Mesh #" << mi << " has "
                      << pMesh->mNumBones << " bones!" << std::endl;
            for (unsigned bi = 0; bi < pMesh->mNumBones; ++bi)
            {
                auto *pBone = pMesh->mBones[bi];
                std::cerr << "bone #" << bi
                          << " '" << pBone->mName.C_Str() << "'"
                          << " (" << pBone->mNumWeights << " weights)"
                          << std::endl;
                PrintAiMatrix4("  ", pBone->mOffsetMatrix);
            }
        }
    }
}

void CAssimpUtils::LoadMaterials(const path &resourceDir,
                                 CAssetLoader &assetLoader,
                                 const aiScene &scene,
                                 std::vector<SPhongMaterial> &materials)
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
