#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "FloatRect.h"
#include "Utils.h"
#include "filesystem_alias.h"

class CTexture2D;
using CTexture2DUniquePtr = std::unique_ptr<CTexture2D>;

enum class TextureWrapMode
{
	REPEAT,
	CLAMP_TO_EDGE,

	// Значение по умолчанию в OpenGL - GL_REPEAT.
	DEFAULT_VALUE = REPEAT,
};

/// Класс владеет текстурой типа GL_TEXTURE_2D,
///    у которой есть две координаты "s" и "t",
///    и позволяет привязывать её к контексту OpenGL
class CTexture2D
{
public:
    CTexture2D(const glm::ivec2 &size, bool hasAlpha);
    ~CTexture2D();

	CTexture2D(const CTexture2D&) = delete;
	CTexture2D& operator=(const CTexture2D&) = delete;

    glm::ivec2 GetSize()const;
    bool HasAlpha()const;

    void Bind()const;
    static void Unbind();

    template <class TFunction>
    void DoWhileBinded(TFunction && fn)const
    {
        Bind();
		// При выходе из функции гарантированно выполняем Unbind.
		try
		{
			fn();
		}
		catch (...)
		{
			Unbind();
			throw;
		}
		Unbind();
    }

private:
    unsigned m_textureId = 0;
    glm::ivec2 m_size;
    bool m_hasAlpha = false;
};

/// Класс загружает текстуру типа GL_TEXTURE_2D,
///    из изображений *.bmp, *.jpg, *.png и любых других форматов,
///    с которыми работает модуль SDL_image.
class CTexture2DLoader
{
public:
    CTexture2DUniquePtr Load(const fs::path &path);

    void SetWrapMode(TextureWrapMode wrap);
    void SetWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT);

private:
    TextureWrapMode m_wrapS = TextureWrapMode::DEFAULT_VALUE;
    TextureWrapMode m_wrapT = TextureWrapMode::DEFAULT_VALUE;
};

/// Класс хранит атлас текстур,
///    который представляет из себя склеенную из нескольких текстур
///    единую текстуру, а также хранит данные для разделения этой
///    текстуры на прямоугольники с помощью текстурных координат.
/// Формат текстур совместим с cocos2d-x, и может быть создан
///    с помощью github.com/sergey-shambir/Cheetah-Texture-Packer
class CTexture2DAtlas
{
public:
    CTexture2DAtlas(const fs::path &xmlPath,
                    CTexture2DLoader loader = CTexture2DLoader());

	CTexture2DAtlas(const CTexture2DAtlas&) = delete;
	CTexture2DAtlas& operator=(const CTexture2DAtlas&) = delete;

    const CTexture2D &GetTexture()const;
    CFloatRect GetFrameRect(const std::string &frameName)const;

private:
    std::unordered_map<std::string, CFloatRect> m_frames;
    CTexture2DUniquePtr m_pTexture;
};
