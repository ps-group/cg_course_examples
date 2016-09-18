#include "stdafx.h"
#include "Window.h"
#include <SDL2/SDL.h>

namespace
{
const glm::ivec2 WINDOW_SIZE = {800, 600};
const char WINDOW_TITLE[] = "OpenGL Demo #7 (WASD camera + animation)";
}

int main(int, char *[])
{
    CWindow window;
    window.Show(WINDOW_TITLE, WINDOW_SIZE);
    window.DoGameLoop();

    return 0;
}
