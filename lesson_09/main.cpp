#include "stdafx.h"
#include "Window.h"

int main()
{
    glewInit();

    CWindow window;
    window.Show({800, 600});
    window.DoGameLoop();
}
