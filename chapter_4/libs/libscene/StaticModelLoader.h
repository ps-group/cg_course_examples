#pragma once
#include "StaticModel3D.h"

class CAssetLoader;

class CStaticModelLoader
{
public:
    CStaticModelLoader(CAssetLoader &assetLoader);

    CStaticModel3DPtr Load(const boost::filesystem::path &path);

private:
    CAssetLoader &m_assetLoader;
};
