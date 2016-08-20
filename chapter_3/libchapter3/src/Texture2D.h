#pragma once
#include <boost/noncopyable.hpp>
#include <boost/scope_exit.hpp>
#include <string>
#include <memory>

class CTexture2D : private boost::noncopyable
{
public:
    CTexture2D();
    ~CTexture2D();

    void Bind()const;
    static void Unbind();

    template <class TFunction>
    void DoWhileBinded(TFunction && fn)const
    {
        Bind();
        // При выходе из функции надо обязательно сделать Unbind.
        BOOST_SCOPE_EXIT_ALL() {
            Unbind();
        };
        fn();
    }

private:
    unsigned m_textureId = 0;
};

using CTexture2DUniquePtr = std::unique_ptr<CTexture2D>;

CTexture2DUniquePtr LoadTexture2DFromBMP(const std::string &path);
