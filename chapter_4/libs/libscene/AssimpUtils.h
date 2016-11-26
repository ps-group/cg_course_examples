#pragma once

// Для импорта используем библиотеку Assimp
//  см. http://assimp.sourceforge.net/
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <glm/fwd.hpp>
#include <boost/filesystem/path.hpp>
#include "../libgeometry/Transform.h"

class CAssetLoader;
struct SPhongMaterial;

enum class SceneImportQuality
{
    Fast,
    HighQuality,
    MaxQuality,
};

class CAssimpUtils
{
public:
    CAssimpUtils() = delete;

    // Открывает сцену из заданного файла
    // Если файла нет, бросает исключение
    // Памятью для хранения возвращаемой сцены владеет importer.
    static const aiScene &OpenScene(const boost::filesystem::path &path,
                                    Assimp::Importer &importer,
                                    SceneImportQuality quality = SceneImportQuality::HighQuality);

    // Выводит подробную информацию о ранее открытой сцене
    static void DumpSceneInfo(const aiScene &scene);

    // Открывает сцену и выводит подробную информацию о ней
    static void DumpSceneInfo(const boost::filesystem::path &path);

    // Загружает материалы сцены в переданный массив.
    static void LoadMaterials(const boost::filesystem::path &resourceDir,
                              CAssetLoader &assetLoader,
                              const aiScene &scene,
                              std::vector<SPhongMaterial> &materials);

    // Печатает матрицу 4x4 в 4 строки с отступом, заданным строкой indent.
    static void PrintGlmMatrix4(const std::string &indent,
                                const glm::mat4 &transform);

    // Конвертирует матрицу формата libassimp в матрицу формата OpenGL/GLM.
    // Процесс включает в себя транспонирование, т.к. матрицы в OpenGL
    //  в размещении в памяти делятся на колонки, а не на строки.
    static glm::mat4 ConvertMat4(const aiMatrix4x4 &value);

    // Конвертирует кватернион формата libassimp
    //  в кватернион формата OpenGL/GLM.
    static glm::quat ConvertQuat(const aiQuaternion &quat);

    // Конвертирует матрицу формата libassimp в CTransform3D.
    // Выбрасывает исключение, если преобразование, заданное матрицей,
    //  не является афинным, либо если имеется преобразование скоса (skew).
    // О преобразовании скоса читайте http://theory.phphtml.net/css/skew.html
    static CTransform3D DecomposeTransform3D(const aiMatrix4x4 &value);
};
