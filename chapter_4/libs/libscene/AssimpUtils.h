#pragma once

// Для импорта используем библиотеку Assimp
//  см. http://assimp.sourceforge.net/
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <boost/filesystem/path.hpp>

class CAssimpUtils
{
public:
    CAssimpUtils() = delete;

    // Памятью для хранения возвращаемой сцены владеет importer.
    static const aiScene &OpenScene(const boost::filesystem::path &path,
                                    Assimp::Importer &importer);

    static void DumpSceneInfo(const aiScene &scene);

    static void DumpSceneInfo(const boost::filesystem::path &path);
};
