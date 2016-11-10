#include "AssimpUtils.h"
#include <sstream>
#include <iostream>

namespace
{
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
