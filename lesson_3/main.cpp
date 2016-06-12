#include "Window.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>

int main()
{
    glewInit();

    CWindow window;
    window.Show({800, 600});
    window.DoGameLoop();
}
