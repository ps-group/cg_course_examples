#pragma once
#include "SkeletalModel3D.h"

class CAssetLoader;

class CSkeletalModelLoader
{
public:
    CSkeletalModelLoader(CAssetLoader &assetLoader);

    CSkeletalModel3DPtr Load(const boost::filesystem::path &path);

private:
    CAssetLoader &m_assetLoader;
};
