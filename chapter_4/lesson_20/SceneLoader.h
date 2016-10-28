#pragma once

#include <anax/World.hpp>
#include <boost/filesystem/path.hpp>

class CSceneLoader
{
public:
    CSceneLoader(anax::World &world);

    void LoadScene();

private:
    anax::World &m_world;
};
