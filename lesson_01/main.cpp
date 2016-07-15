#include "AbstractWindow.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL.h>

// Выключаем макроподмену main на SDL_main,
// т.к. приложение собирается c SUBSYSTEM:CONSOLE
#ifdef _WIN32
#undef main
#endif

class CWindow : public CAbstractWindow
{
protected:
    void OnWindowEvent(const SDL_Event &event) override
    {
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_r:
                SetBackgroundColor({1, 0, 0, 1});
                break;
            case SDLK_g:
                SetBackgroundColor({0, 1, 0, 1});
                break;
            case SDLK_b:
                SetBackgroundColor({0, 0, 1, 1});
                break;
            }
        }
    }

    void OnUpdateWindow(float deltaSeconds) override
    {
        (void)deltaSeconds;
    }

    void OnDrawWindow() override
    {
    }
};

int main()
{
    CWindow window;
    window.ShowFixedSize({800, 600});
    window.DoGameLoop();

    return 0;
}
