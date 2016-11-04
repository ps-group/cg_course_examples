#pragma once

#include <anax/World.hpp>
#include <boost/filesystem/path.hpp>
#include "libscene/AssetLoader.h"

class CCamera;

class CSceneLoader
{
public:
    CSceneLoader(anax::World &world);

    void LoadScene(const boost::filesystem::path &path);

    // Загружает SkyBox из переданного атласа.
    void LoadSkybox(const boost::filesystem::path &path);

private:
    anax::World &m_world;
    CAssetLoader m_assetLoader;
};
