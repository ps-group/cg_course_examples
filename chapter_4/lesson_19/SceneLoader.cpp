#include "stdafx.h"
#include "SceneLoader.h"
#include "json/json.hpp"
#include "Components.h"
#include <fstream>

using namespace nlohmann;
using namespace boost::filesystem;
using glm::vec2;
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

vec2 ReadRange(const json &dict, const std::string &key)
{
    const auto &value = dict.at(key);
    if (!value.is_array())
    {
        throw std::runtime_error("range value '" + key + "' is not an array");
    }

    vec2 result;
    result.x = value.at(0).get<float>();
    result.y = value.at(1).get<float>();

    if (result.y <= result.x)
    {
        throw std::runtime_error("min <= max in range '" + key + "'");
    }

    return result;
}

class CSceneDefinitionParser
{
public:
    CSceneDefinitionParser(anax::World &world, CAssetLoader &assetLoader,
                           const boost::filesystem::path &workdir)
        : m_world(world)
        , m_assetLoader(assetLoader)
        , m_workdir(workdir)
    {
    }

    void ParseParticleSystems(const json &array)
    {
        for (const json &object : array)
        {
            anax::Entity body = m_world.createEntity();
            AddParticleSystem(body, object);
            AddTransform(body, object);
            body.activate();
        };
    }

private:
    std::shared_ptr<CParticleSystem> LoadParticleSystem(const json &dict)
    {
        const path texture = m_workdir / dict.at("texture").get<std::string>();
        const vec3 gravity = ReadOptionalVec3(dict, "gravity", vec3(0));

        auto pSystem = std::make_shared<CParticleSystem>();
        pSystem->SetParticleTexture(m_assetLoader.LoadTexture(texture));
        pSystem->SetEmitter(LoadParticleEmitter(dict.at("emitter")));
        pSystem->SetGravity(gravity);

        return pSystem;
    }

    std::unique_ptr<CParticleEmitter> LoadParticleEmitter(const json &dict)
    {
        const vec3 position = ReadOptionalVec3(dict, "position", vec3(0, 1, 0));
        const vec3 direction = ReadOptionalVec3(dict, "direction", vec3(0, 1, 0));
        const float maxDeviationAngle = dict.at("maxDeviationAngle").get<float>();
        const vec2 distanceRange = ReadRange(dict, "distanceRange");
        const vec2 emitIntervalRange = ReadRange(dict, "emitIntervalRange");
        const vec2 lifetimeRange = ReadRange(dict, "lifetimeRange");
        const vec2 speedRange = ReadRange(dict, "speedRange");

        auto pEmitter = std::make_unique<CParticleEmitter>();
        pEmitter->SetPosition(position);
        pEmitter->SetDirection(direction);
        pEmitter->SetMaxDeviationAngle(maxDeviationAngle);
        pEmitter->SetDistanceRange(distanceRange.x, distanceRange.y);
        pEmitter->SetEmitIntervalRange(emitIntervalRange.x, emitIntervalRange.y);
        pEmitter->SetLifetimeRange(lifetimeRange.x, lifetimeRange.y);
        pEmitter->SetSpeedRange(speedRange.x, speedRange.y);

        return pEmitter;
    }

    void AddParticleSystem(anax::Entity &body, const json &dict)
    {
        auto &com = body.addComponent<CParticleSystemComponent>();
        com.m_pSystem = LoadParticleSystem(dict);
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
    CAssetLoader &m_assetLoader;
    path m_workdir;
    std::unordered_map<std::string, CStaticModel3DPtr> m_modelsCache;
};
}

CSceneLoader::CSceneLoader(anax::World &world)
    : m_world(world)
{
}

void CSceneLoader::LoadScene(const boost::filesystem::path &path)
{
    // Получаем абсолютный путь к файлу описания сцены,
    //  каталог с данным файлом будет использован для поиска ресурсов.
    const auto abspath = m_assetLoader.GetResourceAbspath(path);
    const auto resourceDir = abspath.parent_path();

    std::ifstream file(abspath.native());
    if (!file.is_open())
    {
        throw std::runtime_error("Scene file not found: " + path.generic_string());
    }
    json sceneObj = json::parse(file);

    CSceneDefinitionParser parser(m_world, m_assetLoader, resourceDir);
    parser.ParseParticleSystems(sceneObj["particleSystems"]);
}

#if 0
void CSceneLoader::LoadSkybox(const boost::filesystem::path &path)
{
    CTexture2DAtlas atlas(path, m_assetLoader);
    std::vector<CFloatRect> rects;
    rects.resize(static_cast<unsigned>(CubeFace::NumFaces));
    rects[static_cast<unsigned>(CubeFace::Back)] = atlas.GetFrameRect("skybox-back.jpg");
    rects[static_cast<unsigned>(CubeFace::Front)] = atlas.GetFrameRect("skybox-forward.jpg");
    rects[static_cast<unsigned>(CubeFace::Left)] = atlas.GetFrameRect("skybox-left.jpg");
    rects[static_cast<unsigned>(CubeFace::Right)] = atlas.GetFrameRect("skybox-right.jpg");
    rects[static_cast<unsigned>(CubeFace::Top)] = atlas.GetFrameRect("skybox-top.jpg");
    rects[static_cast<unsigned>(CubeFace::Bottom)] = atlas.GetFrameRect("skybox-bottom.jpg");

    const CStaticGeometry cube = CTesselator::TesselateSkybox(rects);

    auto pModel = std::make_shared<CModel3D>();
    pModel->m_pGeometry = cube.m_pGeometry;
    pModel->m_meshes.emplace_back();
    pModel->m_meshes.back().m_layout = cube.m_layout;
    pModel->m_materials.emplace_back();
    pModel->m_materials.back().pEmissive = atlas.GetTexture();

    anax::Entity skybox = m_world.createEntity();
    auto &mesh = skybox.addComponent<CMeshComponent>();
    mesh.m_category = CMeshComponent::Environment;
    mesh.m_pModel = pModel;
    skybox.addComponent<CTransformComponent>();
    skybox.activate();
}
#endif
