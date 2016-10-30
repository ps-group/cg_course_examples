#include "stdafx.h"
#include "SceneLoader.h"
#include "json/json.hpp"
#include "libchapter4.h"
#include "Components.h"
#include <fstream>

using namespace nlohmann;
using namespace boost::filesystem;
using glm::vec3;

namespace
{

// Считывает vec3 в одном из форматов:
//  - число
//  - массив [x, y, z]
vec3 ReadOptionalVec3(const json &dict, const std::string &key, const vec3 &defaultValue)
{
    vec3 result = defaultValue;

    const auto it = dict.find(key);
    if (it != dict.end())
    {
        const auto &value = *it;
        if (value.is_array())
        {
            result.x = value.at(0).get<float>();
            result.y = value.at(1).get<float>();
            result.z = value.at(2).get<float>();
        }
        if (value.is_number())
        {
            result = vec3(value.get<float>());
        }
    }

    return result;
}

class CSceneDefinitionParser
{
public:
    CSceneDefinitionParser(anax::World &world, CAssetLoader &assetLoader,
                           const boost::filesystem::path &workdir)
        : m_world(world)
        , m_modelLoader(assetLoader)
        , m_workdir(workdir)
    {
    }

    void ParseObjects(const json &array)
    {
        for (const json &object : array)
        {
            anax::Entity body = m_world.createEntity();
            AddMesh(body, object);
            AddTransform(body, object);

            body.activate();
        };
    }

private:
    CComplexMeshSharedPtr LoadModelWithCache(const path &abspath)
    {
        // Пытаемся извлечь модель из кеша.
        auto it = m_modelsCache.find(abspath.generic_string());
        if (it != m_modelsCache.end())
        {
            return it->second;
        }

        // В отладочном режиме выводим информацию о модели.
#if !defined(NDEBUG)
        m_modelLoader.DumpInfo(abspath);
#endif

        SComplexMeshData data;
        m_modelLoader.Load(abspath, data);
        auto pMesh = std::make_shared<CComplexMesh>();
        pMesh->SetData(std::move(data));

        return pMesh;
    }

    void AddMesh(anax::Entity &body, const json &dict)
    {
        const std::string filename = dict.at("model").get<std::string>();
        auto &mesh = body.addComponent<CMeshComponent>();
        mesh.m_pMesh = LoadModelWithCache(m_workdir / filename);
    }

    void AddTransform(anax::Entity &body, const json &dict)
    {
        auto &com = body.addComponent<CTransformComponent>();
        const vec3 sizeScale = ReadOptionalVec3(dict, "size", vec3(1));
        const vec3 position = ReadOptionalVec3(dict, "position", vec3(0));
        const vec3 eulerAngles = ReadOptionalVec3(dict, "eulerAngles", vec3(0));

        com.m_sizeScale = sizeScale;
        com.m_position = position;
        com.m_orientation = glm::quat(eulerAngles);
    }

    anax::World &m_world;
    CModelLoader m_modelLoader;
    path m_workdir;
    std::unordered_map<std::string, CComplexMeshSharedPtr> m_modelsCache;
};
}

CSceneLoader::CSceneLoader(anax::World &world)
    : m_world(world)
{
}

void CSceneLoader::LoadScene(const boost::filesystem::path &path)
{
    CAssetLoader assetLoader;

    // Получаем абсолютный путь к файлу описания сцены,
    //  каталог с данным файлом будет использован для поиска ресурсов.
    const auto abspath = assetLoader.GetResourceAbspath(path);
    const auto resourceDir = abspath.parent_path();

    std::ifstream file(abspath.native());
    if (!file.is_open())
    {
        throw std::runtime_error("Scene file not found: " + path.generic_string());
    }
    json sceneObj = json::parse(file);

    CSceneDefinitionParser parser(m_world, assetLoader, resourceDir);
    parser.ParseObjects(sceneObj["objects"]);
}
