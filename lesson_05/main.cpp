#include "stdafx.h"
#include "Window.h"

int main()
{
    glewInit();

    CWindow window;
    window.Show({1000, 800});
    window.DoGameLoop();
}
