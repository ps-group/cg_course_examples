#define _CRT_SECURE_NO_WARNINGS
#include "Texture2D.h"
#include "../tinyxml/tinyxml2.h"
#include "AssetLoader.h"
#include "libsystem/Utils.h"
#include "includes/glm-common.hpp"
#include "includes/opengl-common.hpp"
#include <codecvt>
#include <cstdlib>
#include <boost/utility/string_ref.hpp>
#include <map>


namespace xml = tinyxml2;
using boost::filesystem::path;


namespace
{

class CPropertyListParser
{
public:
    using MetaHandler = std::function<void(const std::string &textureName, glm::ivec2 &size)>;
    using FrameHandler = std::function<void(const std::string &, const CFloatRect &)>;

    CPropertyListParser(const path &xmlPath, CAssetLoader &loader)
        : m_xmlPath(xmlPath.generic_string())
        , m_loader(loader)
    {
    }

    void DoOnParsedTextureMeta(const MetaHandler &handler)
    {
        m_onParsedTextureMeta = handler;
    }

    void DoOnParsedFrame(const FrameHandler &handler)
    {
        m_onParsedFrame = handler;
    }

    void Parse()
    {
        const std::string xml = m_loader.LoadFileAsString(m_xmlPath);
        xml::XMLDocument document;
        m_error = document.Parse(xml.c_str(), xml.length());
        CheckError();

        const auto *plist = FindChild(&document, "plist");
        const auto *dict = FindChild(plist, "dict");
        ParseMetadata(GetValueNode(dict, "metadata"));
        ParseFrames(GetValueNode(dict, "frames"));
    }

private:
    const xml::XMLElement *FindChild(const xml::XMLNode *parent, const std::string &name)
    {
        const auto *child = parent->FirstChildElement(name.c_str());
        if (!child)
        {
            throw std::logic_error("Child element '" + name + "' not found in '"
                                   + m_xmlPath + "'");
        }
        return child;
    }

    const xml::XMLElement *GetNextSibling(const xml::XMLElement *node)
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

    const xml::XMLElement *GetValueNode(const xml::XMLElement *dict, boost::string_ref key)
    {
        const auto *pKey = dict->FirstChildElement("key");
        while (pKey)
        {
            boost::string_ref currentKey = pKey->GetText();
            if (currentKey == key)
            {
                return GetNextSibling(pKey);
            }
            pKey = pKey->NextSiblingElement("key");
        }

        throw std::logic_error("Key '" + key.to_string() + "' not found in '"
                               + m_xmlPath + "'");
    }

    void ParseMetadata(const xml::XMLElement *metadata)
    {
        const auto *pString = GetValueNode(metadata, "textureFileName");
        const std::string filename = pString->GetText();

        pString = GetValueNode(metadata, "size");
        const std::string sizeStr = pString->GetText();

        glm::ivec2 size;
        if (sscanf(sizeStr.c_str(), "{%d, %d}", &size.x, &size.y) != size.length())
        {
            throw std::logic_error("Invalid size value '" + sizeStr + "' in '"
                                   + m_xmlPath + "'");
        }
        m_onParsedTextureMeta(filename, size);
    }

    void ParseFrames(const xml::XMLElement *frames)
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

    CFloatRect ParseFrameRect(const xml::XMLElement *dict)
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
        if (m_error != xml::XML_SUCCESS)
        {
            const std::string errorCode = std::to_string(unsigned(m_error));
            throw std::runtime_error("Failed to load plist file '" + m_xmlPath
                                     + "': xml error #" + errorCode);
        }
    }

    MetaHandler m_onParsedTextureMeta;
    FrameHandler m_onParsedFrame;
    std::string m_xmlPath;
    CAssetLoader &m_loader;
    xml::XMLError m_error = xml::XML_SUCCESS;
};

GLenum ConvertEnum(TextureWrapMode mode)
{
    static const std::map<TextureWrapMode, GLenum> MAPPING = {
        { TextureWrapMode::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
        { TextureWrapMode::MIRRORED_REPEAT, GL_REPEAT },
        { TextureWrapMode::REPEAT, GL_MIRRORED_REPEAT },
    };
    return MAPPING.at(mode);
}
}


CTexture2D::CTexture2D()
{
    glGenTextures(1, &m_textureId);
}

CTexture2D::CTexture2D(no_texture_tag)
{
}

CTexture2D::~CTexture2D()
{
    if (m_textureId != 0)
    {
        glDeleteTextures(1, &m_textureId);
    }
}

glm::ivec2 CTexture2D::GetSize() const
{
    return m_size;
}

bool CTexture2D::HasAlpha() const
{
    return m_hasAlpha;
}

void CTexture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void CTexture2D::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CTexture2D::ApplyImageData(const SDL_Surface &surface)
{
    m_hasAlpha = SDL_ISPIXELFORMAT_ALPHA(surface.format->format);
    m_size = { surface.w, surface.h };

    const GLenum pixelFormat = m_hasAlpha ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, GLint(pixelFormat), m_size.x, m_size.y,
        0, pixelFormat, GL_UNSIGNED_BYTE, surface.pixels);
}

void CTexture2D::ApplyWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLint(ConvertEnum(wrapS)));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLint(ConvertEnum(wrapT)));
}

void CTexture2D::ApplyTrilinearFilter()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CTexture2D::ApplyMaxAnisotropy()
{
    float anisotropy = 0.f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

void CTexture2D::GenerateMipmaps()
{
    glGenerateMipmap(GL_TEXTURE_2D);
}

CTexture2DSharedPtr CTextureCache::Get(const path &name) const
{
    const auto it = m_cache.find(name.native());
    if (it != m_cache.end())
    {
        return it->second.lock();
    }
    return nullptr;
}

void CTextureCache::Add(const path &name, const CTexture2DSharedPtr &pTexture)
{
    m_cache[name.native()] = pTexture;
}

CTexture2DAtlas::CTexture2DAtlas(const path &xmlPath, CAssetLoader &loader)
{
    const path abspath = loader.GetResourceAbspath(xmlPath);
    glm::vec2 frameScale;

    CPropertyListParser parser(abspath, loader);
    parser.DoOnParsedTextureMeta([&](const std::string &filename, const glm::ivec2 &size) {
        // Запоминаем коэффициенты для преобразования всех координат
        //    в атласе текстур к диапазону [0..1].
        frameScale = { float(1.f / size.x),
                       float(1.f / size.y) };
        m_pTexture = loader.LoadTexture(abspath.parent_path() / filename);
    });
    parser.DoOnParsedFrame([&](const std::string &name, const CFloatRect &rect) {
        // Преобразуем координаты в атласе текстур к диапазону [0..1]
        CFloatRect texRect = rect.GetScaled(frameScale);
        // Переворачиваем по оси Y, чтобы синхронизировать
        //    с переворотом текстуры в загузчике текстур.
        const float flippedY = 1.f - texRect.GetBottomRight().y;
        texRect.MoveTo({ texRect.GetTopLeft().x, flippedY });

        m_frames[name] = texRect;
    });
    parser.Parse();
}

const CTexture2DSharedPtr &CTexture2DAtlas::GetTexture() const
{
    return m_pTexture;
}

CFloatRect CTexture2DAtlas::GetFrameRect(const std::string &frameName) const
{
    try
    {
        return m_frames.at(frameName);
    }
    catch (...)
    {
        throw std::runtime_error("Frame not found: " + frameName);
    }
}
