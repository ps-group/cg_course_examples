#include "stdafx.h"
#include "SceneLoader.h"
#include "libchapter4.h"
#include "Components.h"
#include <fstream>

static const char TANK_MODEL[] = "res/tanks/phoenix_ugv.md2";

CSceneLoader::CSceneLoader(anax::World &world)
    : m_world(world)
{
}

void CSceneLoader::LoadScene()
{
    CAssetLoader assetLoader;
    CModelLoader modelLoader(assetLoader);

    SComplexMeshData data;
    modelLoader.Load(TANK_MODEL, data);

    auto entity = m_world.createEntity();
    auto &meshCom = entity.addComponent<CMeshComponent>();
    meshCom.m_pMesh = std::make_shared<CComplexMesh>();
    meshCom.m_pMesh->SetData(data);
    auto &transformCom = entity.addComponent<CTransformComponent>();
    transformCom.m_sizeScale = glm::vec3(0.1f);
    entity.activate();
}
