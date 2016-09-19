#pragma once

#include <boost/noncopyable.hpp>
#include <boost/scope_exit.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include "FloatRect.h"

class CTexture2D;
using CTexture2DUniquePtr = std::unique_ptr<CTexture2D>;

class CTexture2D : private boost::noncopyable
{
public:
    CTexture2D();
    ~CTexture2D();

    static CTexture2DUniquePtr Load(const boost::filesystem::path &path);

    void Bind()const;
    static void Unbind();

    template <class TFunction>
    void DoWhileBinded(TFunction && fn)const
    {
        Bind();
        // При выходе из функции гарантированно выполняем Unbind.
        BOOST_SCOPE_EXIT_ALL(&) {
            Unbind();
        };
        fn();
    }

private:
    unsigned m_textureId = 0;
};

class CTexture2DAtlas : private boost::noncopyable
{
public:
    CTexture2DAtlas(const boost::filesystem::path &xmlPath);

    const CTexture2D &GetTexture()const;
    CFloatRect GetFrameRect(const std::string &frameName)const;

private:
    std::unordered_map<std::string, CFloatRect> m_frames;
    CTexture2DUniquePtr m_pTexture;
};
