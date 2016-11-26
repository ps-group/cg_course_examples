#pragma once

#include <boost/noncopyable.hpp>
#include <boost/scope_exit.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include "libgeometry/FloatRect.h"
#include "libsystem/Utils.h"

class CAssetLoader;
class CTexture2D;
using CTexture2DSharedPtr = std::shared_ptr<CTexture2D>;
using CTexture2DWeakPtr = std::weak_ptr<CTexture2D>;

enum class TextureWrapMode
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,

    // Значение по умолчанию в OpenGL - GL_REPEAT.
    DEFAULT_VALUE = REPEAT,
};

/// Класс владеет текстурой типа GL_TEXTURE_2D,
///    у которой есть две координаты "s" и "t",
///    и позволяет привязывать её к контексту OpenGL
class CTexture2D : private boost::noncopyable
{
public:
    struct no_texture_tag {};

    CTexture2D();
    CTexture2D(no_texture_tag);
    ~CTexture2D();

    glm::ivec2 GetSize()const;
    bool HasAlpha()const;

    void Bind()const;

    static void Unbind();

    void ApplyImageData(const SDL_Surface &surface);

    void ApplyWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT);

    // См. https://en.wikipedia.org/wiki/Trilinear_filtering
    void ApplyTrilinearFilter();

    // См. https://en.wikipedia.org/wiki/Anisotropic_filtering
    void ApplyMaxAnisotropy();

    void GenerateMipmaps();

private:
    unsigned m_textureId = 0;
    glm::ivec2 m_size;
    bool m_hasAlpha = false;
};

/// Класс предоставляет кеш текстур,
///  достаточно иметь один кеш на приложение,
///  тем не менее, мы не используем паттерн Одиночка (Singletone).
class CTextureCache
{
public:
    /// Возвращает указатель на кешированную текстуру либо nullptr.
    CTexture2DSharedPtr Get(const boost::filesystem::path &name)const;

    /// Добавляет текстуру в кеш текстур.
    void Add(const boost::filesystem::path &name,
             const CTexture2DSharedPtr &pTexture);

private:
    std::unordered_map<boost::filesystem::path::string_type, CTexture2DWeakPtr> m_cache;
};

/// Класс хранит атлас текстур,
///    который представляет из себя склеенную из нескольких текстур
///    единую текстуру, а также хранит данные для разделения этой
///    текстуры на прямоугольники с помощью текстурных координат.
/// Формат текстур совместим с cocos2d-x, и может быть создан
///    с помощью github.com/sergey-shambir/Cheetah-Texture-Packer
class CTexture2DAtlas : private boost::noncopyable
{
public:
    CTexture2DAtlas(const boost::filesystem::path &xmlPath, CAssetLoader &loader);

    const CTexture2DSharedPtr &GetTexture()const;
    CFloatRect GetFrameRect(const std::string &frameName)const;

private:
    std::unordered_map<std::string, CFloatRect> m_frames;
    CTexture2DSharedPtr m_pTexture;
};
