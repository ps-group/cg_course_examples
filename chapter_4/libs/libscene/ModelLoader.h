#pragma once
#include "Model3D.h"

class CAssetLoader;

class CModelLoader
{
public:
    CModelLoader(CAssetLoader &assetLoader);

    CModel3DSharedPtr Load(const boost::filesystem::path &path);
    void DumpInfo(const boost::filesystem::path &path);

private:
    CAssetLoader &m_assetLoader;
};
