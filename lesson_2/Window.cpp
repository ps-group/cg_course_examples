#include "Window.h"
#include <mutex>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <GL/glew.h>
#include <GL/gl.h>

namespace
{
std::once_flag g_didPrintOpenGLInfo;
}

void CWindow::OnWindowEvent(const SDL_Event &event)
{
    (void)event;
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    (void)deltaSeconds;
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    (void)size;
    std::call_once(g_didPrintOpenGLInfo, &CWindow::PrintOpenGLInfo, this);
}

void CWindow::PrintOpenGLInfo()
{
    auto printOpenGLString = [](const char *description, GLenum name) {
        std::string info = reinterpret_cast<const char *>(glGetString(name));
        std::cerr << description << info << std::endl;
    };
    printOpenGLString("OpenGL version: ", GL_VERSION);
    printOpenGLString("OpenGL vendor: ", GL_VENDOR);

    auto testExtension = [](const char *description, GLboolean supportFlag) {
        const char *prefix = supportFlag ? "Has " : "Has no ";
        std::cerr << prefix << description << std::endl;
    };
    testExtension("vertex shaders", GLEW_ARB_vertex_shader);
    testExtension("fragment shaders", GLEW_ARB_fragment_shader);
    testExtension("vertex buffers", GLEW_ARB_vertex_buffer_object);
    testExtension("framebuffers", GLEW_ARB_framebuffer_object);
}
