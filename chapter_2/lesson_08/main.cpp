#include "stdafx.h"
#include "Window.h"
#include <SDL2/SDL.h>

int main(int, char *[])
{
    CWindow window;
    window.Show({800, 600});
    window.DoGameLoop();

    return 0;
}
