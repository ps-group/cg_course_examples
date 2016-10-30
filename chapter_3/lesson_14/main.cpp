#include "stdafx.h"
#include "WindowClient.h"
#include <SDL2/SDL.h>

int main(int, char *[])
{
    try
    {
        CWindow window;
        window.Show("Demo #14: Fragment Shader Drawing", {800, 600});
        CWindowClient client(window);
        window.DoMainLoop();
    }
    catch (const std::exception &ex)
    {
        const char *title = "Fatal Error";
        const char *message = ex.what();
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
    }

    return 0;
}
