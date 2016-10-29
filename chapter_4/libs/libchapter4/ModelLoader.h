#pragma once
#include "ComplexMesh.h"

class CAssetLoader;

class CModelLoader
{
public:
    CModelLoader(CAssetLoader &assetLoader);
    void Load(const boost::filesystem::path &path, SComplexMeshData &data);

    void DumpInfo(const boost::filesystem::path &path);

private:
    CAssetLoader &m_assetLoader;
};
