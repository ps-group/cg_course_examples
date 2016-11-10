#pragma once

// Для импорта используем библиотеку Assimp
//  см. http://assimp.sourceforge.net/
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <glm/fwd.hpp>

#include <boost/filesystem/path.hpp>

class CAssetLoader;
struct SPhongMaterial;

class CAssimpUtils
{
public:
    CAssimpUtils() = delete;

    // Открывает сцену из заданного файла
    // Если файла нет, бросает исключение
    // Памятью для хранения возвращаемой сцены владеет importer.
    static const aiScene &OpenScene(const boost::filesystem::path &path,
                                    Assimp::Importer &importer);

    // Выводит подробную информацию о ранее открытой сцене
    static void DumpSceneInfo(const aiScene &scene);

    // Открывает сцену и выводит подробную информацию о ней
    static void DumpSceneInfo(const boost::filesystem::path &path);

    // Загружает материалы сцены в переданный массив.
    static void LoadMaterials(const boost::filesystem::path &resourceDir,
                              CAssetLoader &assetLoader,
                              const aiScene &scene,
                              std::vector<SPhongMaterial> &materials);

    static glm::mat4 ConvertMat4(const aiMatrix4x4 &value);
};
