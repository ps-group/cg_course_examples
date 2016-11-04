#pragma once

#include "Texture2D.h"
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

class CAssetLoader : private boost::noncopyable
{
public:
    CAssetLoader();
    ~CAssetLoader();

    /// Возвращает гарантированно абслютный путь к файлу ресурса,
    ///  либо выбрасывает исключение.
    boost::filesystem::path GetResourceAbspath(const boost::filesystem::path &path);

    /// Загружает файл с диска как массив байт внутри std::string.
    std::string LoadFileAsString(const boost::filesystem::path &path);

    /// Загружает изображение с диска в SDL_Surface,
    ///  хранящий массив пикселей и вспомогательные данные о формате.
    SDLSurfacePtr LoadFileImage(const boost::filesystem::path &path);

    /// Загружает шрифт по заданному пути и с заданным размером символов.
    TTFFontPtr LoadFixedSizeFont(const boost::filesystem::path &path, int pointSize);

    /// Загружает текстуру типа GL_TEXTURE_2D,
    ///    из изображений *.bmp, *.jpg, *.png и любых других форматов,
    ///    с которыми работает модуль SDL_image.
    CTexture2DSharedPtr LoadTexture(const boost::filesystem::path &path);

private:
    struct Impl;

    std::unique_ptr<Impl> m_impl;
};
