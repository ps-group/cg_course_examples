#pragma once

#include <memory>
#include <boost/noncopyable.hpp>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

class IWindowClient;

class CWindow : private boost::noncopyable
{
public:
    CWindow();
    virtual ~CWindow();

    void SetCoreProfileEnabled(bool enabled);

    void Show(const std::string &title, const glm::ivec2 &size);
    void SetBackgroundColor(glm::vec4 const& color);
    void SetClient(IWindowClient *pClient);
    glm::ivec2 GetWindowSize() const;

    void DoMainLoop();

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};
