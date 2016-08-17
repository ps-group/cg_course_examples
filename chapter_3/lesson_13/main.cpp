#include "stdafx.h"
#include "WindowClient.h"
#include <SDL2/SDL.h>

int main(int, char *[])
{
    CWindow window;
    window.Show("OpenGL Demo", {800, 600});
    CWindowClient client(window);
    window.DoMainLoop();

    return 0;
}
