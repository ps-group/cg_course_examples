#include "stdafx.h"
#include "SceneLoader.h"
#include "json/json.hpp"
#include "libchapter4.h"
#include "MeshP3NT2.h"
#include "Tesselator.h"
#include "Components.h"
#include <fstream>

using namespace nlohmann;

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
glm::vec3 ReadOptionalVec3(const json &dict, const std::string &key, const glm::vec3 &defaultValue)
{
    glm::vec3 result = defaultValue;

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
            result = glm::vec3(value.get<float>());
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
        , m_pSphere(CTesselator::TesselateSphere(SPHERE_PRECISION))
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

            const auto orbitIt = bodyObj.find("orbit");
            if (orbitIt != bodyObj.end())
            {
                AddOrbit(body, orbitIt.value());
            }

            body.activate();
        };
    }

private:
    void AddMesh(anax::Entity &body, const json &dict)
    {
        auto &mesh = body.addComponent<CStaticMeshComponent>();
        mesh.m_pEmissiveMap = GetOptTexture(dict, "emissive");
        mesh.m_pDiffuseMap = GetOptTexture(dict, "diffuse");
        mesh.m_pSpecularMap = GetOptTexture(dict, "specular");
        mesh.m_pMesh = m_pSphere;
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
        com.m_sizeScale = ReadOptionalVec3(dict, "scale", glm::vec3(1));
        com.m_position = ReadOptionalVec3(dict, "position", glm::vec3(0));
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
    std::shared_ptr<CMeshP3NT2> m_pSphere;
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

    CSceneDefinitionParser impl(m_world, assetLoader, resourceDir);
    impl.ParseSpaceObjects(sceneObj["space_objects"]);
}
