#include "stdafx.h"
#include "AbstractWindow.h"
#include "Utils.h"

namespace
{
const char WINDOW_TITLE[] = "SDL2+OpenGL Demo (press R, G, B to change window color)";
}

class CAbstractWindow::Impl
{
public:
    void ShowFixedSize(glm::ivec2 const& size)
	{
		CUtils::InitOnceSDL2();

        // Специальное значение SDL_WINDOWPOS_CENTERED вместо x и y заставит SDL2
        // разместить окно в центре монитора по осям x и y.
        // Для использования OpenGL вы ДОЛЖНЫ указать флаг SDL_WINDOW_OPENGL.
        m_pWindow.reset(SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         size.x, size.y, SDL_WINDOW_OPENGL));

        // Создаём контекст OpenGL, связанный с окном.
        m_pGLContext.reset(SDL_GL_CreateContext(m_pWindow.get()));
        if (!m_pGLContext)
        {
			CUtils::ValidateSDL2Errors();
        }
    }

    void SetBackgroundColor(const glm::vec4 &color)
    {
        m_clearColor = color;
    }

    void Clear()const
    {
        // Заливка кадра цветом фона средствами OpenGL
        glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void SwapBuffers()
    {
        // Вывод нарисованного кадра в окно на экране.
        // При этом система отдаёт старый буфер для рисования нового кадра.
        // Обмен двух буферов вместо создания новых позволяет не тратить ресурсы впустую.
        SDL_GL_SwapWindow(m_pWindow.get());
    }

private:
    SDLWindowPtr m_pWindow;
    SDLGLContextPtr m_pGLContext;
    glm::vec4 m_clearColor;
};

CAbstractWindow::CAbstractWindow()
    : m_pImpl(new Impl)
{
}

CAbstractWindow::~CAbstractWindow()
{
}

void CAbstractWindow::ShowFixedSize(const glm::ivec2 &size)
{
    m_pImpl->ShowFixedSize(size);
}

void CAbstractWindow::DoGameLoop()
{
    SDL_Event event;
    CChronometer chronometer;
    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else
            {
                OnWindowEvent(event);
            }
        }
        // Очистка буфера кадра, обновление и рисование сцены, вывод буфера кадра.
        if (running)
        {
            m_pImpl->Clear();
            const float deltaSeconds = chronometer.GrabDeltaTime();
            OnUpdateWindow(deltaSeconds);
            OnDrawWindow();
            m_pImpl->SwapBuffers();
        }
    }
}

void CAbstractWindow::SetBackgroundColor(const glm::vec4 &color)
{
    m_pImpl->SetBackgroundColor(color);
}
