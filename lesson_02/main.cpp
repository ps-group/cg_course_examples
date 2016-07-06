#include "Window.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>

int main()
{
    CWindow window;
    window.ShowFixedSize({800, 600});
    window.DoGameLoop();
}
