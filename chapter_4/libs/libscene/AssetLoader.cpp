#include "AssetLoader.h"
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <codecvt>
#include <map>
#ifdef _WIN32
#include <sdkddkver.h>
#include <Windows.h>
#endif

using namespace boost::filesystem;

namespace
{
const size_t FILE_RESERVE_SIZE = 4096;
const size_t MAX_PATH_SIZE = 4096;

// Возвращает путь к текущему исполняемому файлу,
// .exe на Windows, ELF на Linux, MachO на MacOSX
std::string GetExecutablePath()
{
    char buffer[MAX_PATH_SIZE];
    size_t size = sizeof(buffer);
#if defined(_WIN32)
    size = size_t(::GetModuleFileNameA(nullptr, buffer, DWORD(size)));
#elif defined(__linux__)
    ssize_t result = readlink("/proc/self/exe", buffer, size);
    if (result <= 0)
    {
        throw std::runtime_error("Unexpected error on readlink()");
    }
    size = size_t(result);
#elif defined(__apple__)
    uint32_t sizeU32 = uint32_t(size);
    if (_NSGetExecutablePath(buffer, &sizeU32) != 0)
    {
        throw std::runtime_error("Unexpected error on _NSGetExecutablePath()");
    }
    size = size_t(sizeU32);
#endif
    return std::string(buffer, size_t(size));
}

// Функция преобразует путь в родном для системы формате в UTF-8.
//  Библиотека SDL2 принимает пути в UTF-8 на всех платформах.
std::string ConvertPathToUtf8(const boost::filesystem::path &path)
{
#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(path.native());
#else // unix-платформы.
    return path.native();
#endif
}
}

struct CAssetLoader::Impl
{
    Impl()
    {
        m_searchPaths = {
            path(GetExecutablePath()).parent_path(),
            current_path()
        };
    }

    path GetResourceAbspath(const path &relpath)const
    {
        if (relpath.is_absolute())
        {
            return relpath;
        }

        for (const auto &dir : m_searchPaths)
        {
            const path abspath = absolute(relpath, dir);
            if (exists(abspath))
            {
                return abspath;
            }
        }
        throw std::runtime_error("Resource not found: " + relpath.generic_string());
    }

    std::string LoadFileAsString(const path &relpath)
    {
        const path abspath = GetResourceAbspath(relpath);

        std::ifstream input;
        input.open(abspath.native());
        if (!input.is_open())
        {
            throw std::runtime_error("Cannot open for reading: " + abspath.generic_string());
        }

        std::string text;
        text.reserve(FILE_RESERVE_SIZE);
        input.exceptions(std::ios::badbit);

        std::string line;
        while (std::getline(input, line))
        {
            text.append(line);
            text.append("\n");
        }

        return text;
    }

    SDLSurfacePtr LoadFileImage(const path &path)const
    {
        const std::string pathUtf8 = ConvertPathToUtf8(GetResourceAbspath(path));
        SDLSurfacePtr pSurface(IMG_Load(pathUtf8.c_str()));
        if (!pSurface)
        {
            throw std::runtime_error("Cannot find texture at " + path.generic_string());
        }

        return pSurface;
    }

    TTFFontPtr LoadFixedSizeFont(const path &path, int pointSize)const
    {
        const std::string pathUtf8 = ConvertPathToUtf8(GetResourceAbspath(path));
        TTFFontPtr pFont(TTF_OpenFont(pathUtf8.c_str(), pointSize));
        if (!pFont)
        {
            throw std::runtime_error("Cannot find font at " + path.generic_string());
        }

        return pFont;
    }

    CTexture2DSharedPtr LoadTexture(const path &relpath)const
    {
        const path abspath = GetResourceAbspath(relpath);
        CTexture2DSharedPtr pTexture = m_textureCache.Get(abspath);
        if (!pTexture)
        {
            pTexture = LoadFromDisk(abspath);
            m_textureCache.Add(abspath, pTexture);
        }
        return pTexture;
    }

private:
    CTexture2DSharedPtr LoadFromDisk(const path &path)const
    {
        SDLSurfacePtr pSurface = LoadFileImage(path);
        const bool hasAlpha = SDL_ISPIXELFORMAT_ALPHA(pSurface->format->format);

        // Все изображения будем конвертировать в RGB или RGBA,
        //  в зависимости от наличия альфа-канала в исходном изображении.
        const uint32_t requiredFormat = hasAlpha
            ? SDL_PIXELFORMAT_ABGR8888
            : SDL_PIXELFORMAT_RGB24;
        if (pSurface->format->format != requiredFormat)
        {
            pSurface.reset(SDL_ConvertSurfaceFormat(pSurface.get(), requiredFormat, 0));
        }

        // В системе координат OpenGL отсчёт идёт с нижней левой точки,
        //  а не с верхней левой, поэтому переворачиваем изображение.
        CUtils::FlipSurfaceVertically(*pSurface);

        auto pTexture = std::make_shared<CTexture2D>();
        pTexture->Bind();
        pTexture->ApplyImageData(*pSurface);
        pTexture->ApplyTrilinearFilter();
        pTexture->ApplyMaxAnisotropy();
        pTexture->ApplyWrapMode(m_wrapS, m_wrapT);
        pTexture->GenerateMipmaps();
        pTexture->Unbind();

        return pTexture;
    }

    std::vector<boost::filesystem::path> m_searchPaths;
    mutable CTextureCache m_textureCache;
    TextureWrapMode m_wrapS = TextureWrapMode::DEFAULT_VALUE;
    TextureWrapMode m_wrapT = TextureWrapMode::DEFAULT_VALUE;
};

CAssetLoader::CAssetLoader()
    : m_impl(std::make_unique<Impl>())
{
}

CAssetLoader::~CAssetLoader()
{
}

path CAssetLoader::GetResourceAbspath(const path &path)
{
    return m_impl->GetResourceAbspath(path);
}

std::string CAssetLoader::LoadFileAsString(const path &path)
{
    return m_impl->LoadFileAsString(path);
}

SDLSurfacePtr CAssetLoader::LoadFileImage(const path &path)
{
    return m_impl->LoadFileImage(path);
}

TTFFontPtr CAssetLoader::LoadFixedSizeFont(const path &path, int pointSize)
{
    return m_impl->LoadFixedSizeFont(path, pointSize);
}

CTexture2DSharedPtr CAssetLoader::LoadTexture(const path &path)
{
    return m_impl->LoadTexture(path);
}
