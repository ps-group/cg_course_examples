#pragma once
#include "ComplexMesh.h"

class CAssetLoader;

class CModelLoader
{
public:
    CModelLoader(CAssetLoader &assetLoader);
    void Load(const boost::filesystem::path &path, SComplexMeshData &data);

private:
    CAssetLoader &m_assetLoader;
};
