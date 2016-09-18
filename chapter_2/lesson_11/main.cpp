#include "stdafx.h"
#include "Window.h"
#include <SDL2/SDL.h>

namespace
{
const glm::ivec2 WINDOW_SIZE = {800, 600};
const char WINDOW_TITLE[] = "OpenGL Demo #7 (WASD camera + animation)";
const char ERROR_TITLE[] = "Fatal Error";
}

int main(int, char *[])
{
    try
    {
        CWindow window;
        window.Show(WINDOW_TITLE, WINDOW_SIZE);
        window.DoGameLoop();
    }
    catch (const std::exception &ex)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, ERROR_TITLE,
                                 ex.what(), nullptr);
    }

    return 0;
}
