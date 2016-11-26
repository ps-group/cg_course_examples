#include "AssimpUtils.h"
#include "includes/glm-common.hpp"
#include "Geometry.h"
#include "AssetLoader.h"
#include <sstream>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>

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

        const std::string meshIds = JoinMeshIds(*pNode);
        PrintNodeInfo(pNode->mName.C_Str(), pNode->mTransformation, meshIds);
        for (unsigned i = 0, n = pNode->mNumChildren; i < n; ++i)
        {
            Inspect(pNode->mChildren[i]);
        }
        m_indentLevel -= 1;
    }

private:
    std::string JoinMeshIds(const aiNode &node)const
    {
        if (node.mNumMeshes == 0)
        {
            return std::string();
        }

        std::vector<unsigned> ids(node.mMeshes, node.mMeshes + node.mNumMeshes);

        std::string text = "(mesh ids";
        for (unsigned mesh : ids)
        {
            text += " ";
            text += std::to_string(mesh);
        }
        text += ") ";

        return text;
    }

    void PrintNodeInfo(const std::string &name,
                       const aiMatrix4x4 &transform,
                       const std::string &meshIds)
    {

        const std::string indent(2 * m_indentLevel, ' ');
        std::cerr << indent << "Node " << meshIds << name << std::endl;
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

const aiScene &CAssimpUtils::OpenScene(const boost::filesystem::path &path, Assimp::Importer &importer, SceneImportQuality quality)
{
    // Выбираем один из предопределённых в Assimp наборов флагов
    //  для рендеринга в реальном времени с указанным уровнем качества.
    unsigned importFlags = 0;
    switch (quality)
    {
    case SceneImportQuality::Fast:
        importFlags = aiProcessPreset_TargetRealtime_Fast;
        break;
    case SceneImportQuality::HighQuality:
        importFlags = aiProcessPreset_TargetRealtime_Quality;
        break;
    case SceneImportQuality::MaxQuality:
        importFlags = aiProcessPreset_TargetRealtime_MaxQuality;
        break;
    }

    // Загружаем сцену из указанного файла.
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

    // Вывод информации о костях.
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

    // Вывод информации об анимациях
    for (unsigned ai = 0; ai < scene.mNumAnimations; ++ai)
    {
        const aiAnimation &anim = *scene.mAnimations[ai];
        std::cerr << "Animation '" << anim.mName.C_Str()
                  << "' has duration = " << anim.mDuration << " ticks"
                  << " and ticks/sec = " << anim.mTicksPerSecond
                  << ", " << anim.mNumMeshChannels << " mesh channels"
                  << ", " << anim.mNumChannels << " node channels"
                  << std::endl;
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

glm::mat4 CAssimpUtils::ConvertMat4(const aiMatrix4x4 &value)
{
    // В OpenGL матрицы по умолчанию принимаются в виде
    //  "столбец за столбцом", а не "строка за строкой", поэтому мы
    //  создаём матрицу из массива float и затем транспонируем её.
    return glm::transpose(glm::make_mat4(&value.a1));
}

glm::quat CAssimpUtils::ConvertQuat(const aiQuaternion &quat)
{
    // Конструктор glm::quat принимает параметры в порядке (w, x, y, z),
    //  но внутри класса хранятия (x, y, z, w),
    //  поэтому мы не используем glm::make_quat.
    return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

void CAssimpUtils::PrintGlmMatrix4(const std::string &indent, const glm::mat4 &transform)
{
    const unsigned SIZE = 4;
    for (unsigned row = 0; row < SIZE; ++row)
    {
        std::cerr << indent << "[ ";
        for (unsigned column = 0; column < SIZE; ++column)
        {
            const float value = transform[column][row];
            std::cerr << value << " ";
        }
        std::cerr << "]" << std::endl;
    }
}

CTransform3D CAssimpUtils::DecomposeTransform3D(const aiMatrix4x4 &value)
{
    const float EPSILON = 0.0001f;
    const glm::mat4 affinityMat4 = ConvertMat4(value);

    glm::vec3 skew;
    glm::vec4 perspective;
    CTransform3D transform;
    bool ok = glm::decompose(affinityMat4,
                             transform.m_sizeScale,
                             transform.m_orientation,
                             transform.m_position,
                             skew,
                             perspective);

    transform.m_orientation = glm::conjugate(transform.m_orientation);

    if (!ok)
    {
        throw std::runtime_error("aiMatrix4x4 decomposition failed");
    }
    if (glm::length(skew) >= EPSILON)
    {
        throw std::runtime_error("Cannot convert aiMatrix4x4 to CTransform3D: skew transform exists");
    }
    if (glm::length(perspective - vec4(0, 0, 0, 1)) >= EPSILON)
    {
        throw std::runtime_error("Cannot convert aiMatrix4x4 to CTransform3D: matrix is not affinity");
    }

    return transform;
}
