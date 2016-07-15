#include "Window.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>
#include <SDL2/SDL.h>

int main(int, char *[])
{
    glewInit();

    CWindow window;
    window.Show({800, 600});
    window.DoGameLoop();

    return 0;
}
