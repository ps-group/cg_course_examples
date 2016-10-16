#pragma once

#include <anax/World.hpp>
#include <boost/filesystem/path.hpp>

class CSceneLoader
{
public:
    CSceneLoader(anax::World &world);

    void LoadScene(const boost::filesystem::path &path);

private:
    anax::World &m_world;
};
