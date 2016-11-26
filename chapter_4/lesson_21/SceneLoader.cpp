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
const int SPHERE_PRECISION = 40;
const double DAYS_IN_YEAR_ON_EARTH = 365.24;

double ReadNumber(const json &dict, const std::string &key)
{
    return dict.at(key).get<double>();
}

double ReadOptionalNumber(const json &dict, const std::string &key, double defaultValue)
{
    const auto it = dict.find(key);
    if (it != dict.end())
    {
        return it->get<double>();
    }
    return defaultValue;
}

// Считывает диапазон значений в формате [min (число), max (число)]
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

// Считывает угод в одном из форматов:
//   - [градусы (число), секунды (число)]
//   - градусы (число)
double ReadAngle(const json &dict, const std::string &key)
{
    const auto &value = dict.at(key);

    double degrees = 0;
    double seconds = 0;
    if (value.is_array())
    {
        degrees = value.at(0).get<double>();
        seconds = value.at(1).get<double>();
    }
    if (value.is_number())
    {
        degrees = value.get<double>();
    }

    return glm::radians(degrees + seconds / 60.0);
}

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
        : m_assetLoader(assetLoader)
        , m_world(world)
        , m_workdir(workdir)
        , m_sphereGeometry(CTesselator::TesselateSphere(SPHERE_PRECISION))
    {
    }

    void ParseSpaceObjects(const json &dict)
    {
        for (json::const_iterator it = dict.begin(); it != dict.end(); ++it)
        {
            const std::string name = it.key();
            const json &bodyObj = it.value();
            anax::Entity body = m_world.createEntity();
            AddMesh(body, bodyObj);
            AddSpaceBody(body, name, bodyObj);
            AddTransform(body, bodyObj);

            const auto particlesIt = bodyObj.find("particleSystem");
            if (particlesIt != bodyObj.end())
            {
                AddParticleSystem(body, particlesIt.value());
            }

            const auto orbitIt = bodyObj.find("orbit");
            if (orbitIt != bodyObj.end())
            {
                AddOrbit(body, orbitIt.value());
            }

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
        com.m_particleScale = dict.at("particleScale").get<float>();
    }

    void AddMesh(anax::Entity &body, const json &dict)
    {
        auto &mesh = body.addComponent<CMeshComponent>();
        mesh.m_pEmissive = GetOptTexture(dict, "emissive");
        mesh.m_pDiffuse = GetOptTexture(dict, "diffuse");
        mesh.m_pSpecular = GetOptTexture(dict, "specular");
        mesh.m_geometry = m_sphereGeometry;
        mesh.m_category = CMeshComponent::Foreground;
    }

    void AddSpaceBody(anax::Entity &body, const std::string &name, const json &dict)
    {
        const double dayDurationInYears =
                ReadOptionalNumber(dict, "dayDuration", 0)
                / DAYS_IN_YEAR_ON_EARTH;
        const double bodySize = ReadOptionalNumber(dict, "size", 0);

        auto &com = body.addComponent<CSpaceBodyComponent>();
        com.m_name = name;
        com.m_bodySize = float(bodySize);
        com.m_dayDuration = float(dayDurationInYears);
        com.m_rotationAxis = ReadOptionalVec3(dict, "rotationAxis", {0, 1, 0});
    }

    void AddTransform(anax::Entity &body, const json &dict)
    {
        auto &com = body.addComponent<CTransformComponent>();
        com.m_sizeScale = ReadOptionalVec3(dict, "scale", vec3(1));
        com.m_position = ReadOptionalVec3(dict, "position", vec3(0));
    }

    void AddOrbit(anax::Entity &body, const json &dict)
    {
        const double largeAxis = ReadNumber(dict, "largeAxis");
        const double eccentricity = ReadNumber(dict, "eccentricity");
        const double meanMotion = 1.0 / ReadNumber(dict, "yearDuration");
        const double periapsisEpoch = ReadAngle(dict, "periapsisEpoch");

        auto &com = body.addComponent<CEllipticOrbitComponent>(
                    largeAxis,
                    eccentricity,
                    meanMotion,
                    periapsisEpoch);
        com.m_ownerName = dict.at("owner").get<std::string>();
    }

    // Возвращает указатель на текстуру
    // либо nullptr, если текстура не указана в JSON
    CTexture2DSharedPtr GetOptTexture(const json &dict, const std::string &key)
    {
        const auto it = dict.find(key);
        if (it != dict.end() && it.value().is_string())
        {
            const std::string filename = it->get<std::string>();
            return m_assetLoader.LoadTexture(m_workdir / filename);
        }
        return nullptr;
    }

    CAssetLoader &m_assetLoader;
    anax::World &m_world;
    boost::filesystem::path m_workdir;
    CStaticGeometry m_sphereGeometry;
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
    parser.ParseSpaceObjects(sceneObj["space_objects"]);
}

void CSceneLoader::LoadSkybox(const boost::filesystem::path &path)
{
    CTexture2DAtlas atlas(path, m_assetLoader);
    std::vector<CFloatRect> rects;
    rects.resize(static_cast<unsigned>(CubeFace::NumFaces));
    rects[static_cast<unsigned>(CubeFace::Back)] = atlas.GetFrameRect("stars_bk.jpg");
    rects[static_cast<unsigned>(CubeFace::Front)] = atlas.GetFrameRect("stars_fr.jpg");
    rects[static_cast<unsigned>(CubeFace::Left)] = atlas.GetFrameRect("stars_lf.jpg");
    rects[static_cast<unsigned>(CubeFace::Right)] = atlas.GetFrameRect("stars_rt.jpg");
    rects[static_cast<unsigned>(CubeFace::Top)] = atlas.GetFrameRect("stars_up.jpg");
    rects[static_cast<unsigned>(CubeFace::Bottom)] = atlas.GetFrameRect("stars_dn.jpg");

    anax::Entity skybox = m_world.createEntity();
    auto &mesh = skybox.addComponent<CMeshComponent>();
    mesh.m_category = CMeshComponent::Environment;
    mesh.m_geometry = CTesselator::TesselateSkybox(rects);
    mesh.m_pEmissive = atlas.GetTexture();

    skybox.addComponent<CTransformComponent>();
    skybox.activate();
}
