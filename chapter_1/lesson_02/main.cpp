#include "stdafx.h"
#include "Window.h"
#include <SDL2/SDL.h>

// Выключаем макроподмену main на SDL_main,
// т.к. приложение собирается c SUBSYSTEM:CONSOLE
#ifdef _WIN32
#undef main
#endif

int main()
{
    CWindow window;
    window.ShowFixedSize({800, 600});
    window.DoGameLoop();

    return 0;
}
