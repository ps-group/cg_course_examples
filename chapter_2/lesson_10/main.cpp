#include "stdafx.h"
#include "Window.h"
#include <SDL2/SDL.h>

namespace
{
const glm::ivec2 WINDOW_SIZE = {800, 600};
const char WINDOW_TITLE[] = "OpenGL Demo #10 (function of two variables)";
}

int main(int, char *[])
{
    CWindow window;
    window.Show(WINDOW_TITLE, WINDOW_SIZE);
    window.DoGameLoop();

    return 0;
}
