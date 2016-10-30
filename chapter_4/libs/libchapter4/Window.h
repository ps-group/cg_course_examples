#pragma once

#include <memory>
#include <boost/noncopyable.hpp>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <string>

class IWindowClient;

enum class ContextProfile : uint8_t
{
    // Compatibility profile without exact version
    Compatibility,
    // OpenGL 3.1 with forward compatibility.
    RobustOpenGL_3_1,
    // OpenGL 3.2 with forward compatibility.
    RobustOpenGL_3_2,
    // OpenGL 4.0 with forward compatibility.
    RobustOpenGL_4_0,
};

enum class ContextMode : uint8_t
{
    // No special context settings.
    Normal,
    // Use debug context.
    Debug,
};

class CWindow : private boost::noncopyable
{
public:
    CWindow(ContextProfile profile = ContextProfile::Compatibility,
            ContextMode mode = ContextMode::Normal);
    virtual ~CWindow();

    void Show(const std::string &title, const glm::ivec2 &size);
    void ShowFullscreen(const std::string &title);
    void WarpMouse(const glm::ivec2 &newPosition);

    void SetBackgroundColor(glm::vec4 const& color);
    void SetClient(IWindowClient *pClient);
    glm::ivec2 GetWindowSize() const;

    void DoMainLoop();

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};
