#pragma once
#include "ComplexMesh.h"

class CModelLoader
{
public:
    using LoadTextureFn = std::function<CTexture2DSharedPtr(const boost::filesystem::path &)>;

    CModelLoader(const LoadTextureFn &loadTextureFn);

    void Load(const boost::filesystem::path &path, SComplexMeshData &data);

private:
    LoadTextureFn m_loadTextureFn;
};
