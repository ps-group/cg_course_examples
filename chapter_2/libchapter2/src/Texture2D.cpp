#include "libchapter2_private.h"
#include "Texture2D.h"
#include "tinyxml2.h"
#include "FilesystemUtils.h"
#include <codecvt>
#include <SDL2/SDL_image.h>
#include <cstdlib>
#include <boost/utility/string_ref.hpp>


using namespace tinyxml2;


namespace
{
struct SDLSurfaceDeleter
{
	void operator()(SDL_Surface *ptr)
	{
		SDL_FreeSurface(ptr);
	}
};
// Используем unique_ptr с явно заданной функцией удаления вместо delete.
using SDLSurfacePtr = std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>;

// Convert boost::filesystem::path to UTF-8 string, acceptable by SDL_image.
std::string ConvertPathToUtf8(const boost::filesystem::path &path)
{
#ifdef _WIN32
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(path.native());
#else
	return path.native();
#endif
}

void FlipSurfaceVertically(SDL_Surface & surface)
{
	const auto rowSize = size_t(surface.w * surface.format->BytesPerPixel);
	std::vector<uint8_t> row(rowSize);

	for (size_t y = 0, height = size_t(surface.h); y < height / 2; ++y)
	{
		auto *pixels = reinterpret_cast<uint8_t*>(surface.pixels);
		auto *upperRow = pixels + rowSize * y;
		auto *lowerRow = pixels + rowSize * (height - y - 1);
		std::memcpy(row.data(), upperRow, rowSize);
		std::memcpy(upperRow, lowerRow, rowSize);
		std::memcpy(lowerRow, row.data(), rowSize);
	}
}

class CPropertyListParser
{
public:
    using StringHandler = std::function<void(const std::string &)>;
    using FrameHandler = std::function<void(const std::string &, const CFloatRect &)>;

    CPropertyListParser(const boost::filesystem::path &xmlPath)
        : m_xmlPath(xmlPath.generic_string())
    {
    }

    void DoOnParsedTextureFilename(const StringHandler &handler)
    {
        m_onParsedTextureFilename = handler;
    }

    void DoOnParsedFrame(const FrameHandler &handler)
    {
        m_onParsedFrame = handler;
    }

    void Parse()
    {
        const std::string xml = CFilesystemUtils::LoadFileAsString(m_xmlPath);
        XMLDocument document;
        m_error = document.Parse(xml.c_str(), xml.length());
        CheckError();

        const auto *plist = FindChild(&document, "plist");
        const auto *dict = FindChild(plist, "dict");
        ParseFrames(FindChild(dict, "frames"));
        ParseMetadata(FindChild(dict, "metadata"));
    }

private:
    const XMLElement *FindChild(const XMLNode *parent, const std::string &name)
    {
        const auto *child = parent->FirstChildElement(name.c_str());
        if (!child)
        {
            throw std::logic_error("Child element '" + name + "' not found in '"
                                   + m_xmlPath + "'");
        }
        return child;
    }

    const XMLElement *GetNextSibling(const XMLElement *node)
    {
        const auto *sibling = node->NextSiblingElement();
        if (!sibling)
        {
            std::string name = node->Name();
            throw std::logic_error("Sibling for '" + name + "' not found in '"
                                   + m_xmlPath + "'");
        }
        return sibling;
    }

    const XMLElement *GetValueNode(const XMLElement *dict, boost::string_ref key)
    {
        const auto *pKey = dict->FirstChildElement("key");
        while (pKey)
        {
            boost::string_ref currentKey = pKey->GetText();
            if (currentKey == key)
            {
                return GetNextSibling(dict);
            }
            pKey = pKey->NextSiblingElement("key");
        }

        throw std::logic_error("Key '" + key.to_string() + "' not found in '"
                               + m_xmlPath + "'");
    }

    void ParseMetadata(const XMLElement *metadata)
    {
        const auto *pString = GetValueNode(metadata, "textureFileName");
        const std::string filename = pString->GetText();
        m_onParsedTextureFilename(filename);
    }

    void ParseFrames(const XMLElement *frames)
    {
        const auto *pKey = frames->FirstChildElement("key");
        while (pKey)
        {
            const std::string spriteName = pKey->GetText();
            const auto *dict = GetNextSibling(pKey);
            CFloatRect rect = ParseFrameRect(dict);
            m_onParsedFrame(spriteName, rect);
            pKey = pKey->NextSiblingElement("key");
        }
    }

    CFloatRect ParseFrameRect(const XMLElement *dict)
    {
        const auto *nodeX = GetValueNode(dict, "x");
        const auto *nodeY = GetValueNode(dict, "y");
        const auto *nodeWidth = GetValueNode(dict, "width");
        const auto *nodeHeight = GetValueNode(dict, "height");

        const auto x = std::stoul(nodeX->GetText());
        const auto y = std::stoul(nodeY->GetText());
        const auto width = std::stoul(nodeWidth->GetText());
        const auto height = std::stoul(nodeHeight->GetText());

        return CFloatRect(glm::vec2{float(x), float(y)},
                          glm::vec2{float(x + width), float(y + height)});
    }

    void CheckError()
    {
        if (m_error != XML_SUCCESS)
        {
            const std::string errorCode = std::to_string(unsigned(m_error));
            throw std::runtime_error("Failed to load plist file '" + m_xmlPath
                                     + "': xml error #" + errorCode);
        }
    }

    StringHandler m_onParsedTextureFilename;
    FrameHandler m_onParsedFrame;
    std::string m_xmlPath;
    XMLError m_error = XML_SUCCESS;
};
}


CTexture2D::CTexture2D()
{
    glGenTextures(1, &m_textureId);
}

CTexture2D::~CTexture2D()
{
    glDeleteTextures(1, &m_textureId);
}

CTexture2DUniquePtr CTexture2D::Load(const boost::filesystem::path &path)
{
    const std::string pathUtf8 = ConvertPathToUtf8(path);
    SDLSurfacePtr pSurface(IMG_Load(pathUtf8.c_str()));
    if (!pSurface)
    {
        throw std::runtime_error("Cannot find texture at " + path.generic_string());
    }

    const GLenum pixelFormat = GL_RGB;
    switch (pSurface->format->format)
    {
    case SDL_PIXELFORMAT_RGB24:
        break;
    case SDL_PIXELFORMAT_BGR24:
        pSurface.reset(SDL_ConvertSurfaceFormat(pSurface.get(),
                                                SDL_PIXELFORMAT_RGB24, 0));
        break;
    default:
    {
        std::string name = SDL_GetPixelFormatName(pSurface->format->format);
        throw std::runtime_error("Unsupported image pixel format " + name
                                 + " at " + path.generic_string());
    }
    }

    FlipSurfaceVertically(*pSurface);

    auto pTexture = std::make_unique<CTexture2D>();
    pTexture->DoWhileBinded([&] {
        glTexImage2D(GL_TEXTURE_2D, 0, GLint(pixelFormat), pSurface->w, pSurface->h,
                     0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    });

    return pTexture;
}

void CTexture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void CTexture2D::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

CTexture2DAtlas::CTexture2DAtlas(const boost::filesystem::path &xmlPath)
{
    CPropertyListParser parser(xmlPath);
    parser.DoOnParsedTextureFilename([&](const std::string &filename) {
        m_pTexture = CTexture2D::Load(xmlPath.parent_path() / filename);
    });
    parser.DoOnParsedFrame([&](const std::string &name, const CFloatRect &rect) {
        m_frames[name] = rect;
    });
    parser.Parse();
}

const CTexture2D &CTexture2DAtlas::GetTexture() const
{
    return *m_pTexture;
}
