#include "Window.h"
#include "AbstractWindowClient.h"
#include "Utils.h"
#include "libchapter3_private.h"

namespace
{
glm::vec2 GetMousePosition(const SDL_MouseButtonEvent &event)
{
    return { event.x, event.y };
}

glm::vec2 GetMousePosition(const SDL_MouseMotionEvent &event)
{
    return { event.x, event.y };
}

void DispatchEvent(const SDL_Event &event, IWindowClient &acceptor)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        acceptor.OnKeyDown(event.key);
        break;
    case SDL_KEYUP:
        acceptor.OnKeyUp(event.key);
        break;
    case SDL_MOUSEBUTTONDOWN:
        acceptor.OnDragBegin(GetMousePosition(event.button));
        break;
    case SDL_MOUSEBUTTONUP:
        acceptor.OnDragEnd(GetMousePosition(event.button));
        break;
    case SDL_MOUSEMOTION:
        acceptor.OnDragMotion(GetMousePosition(event.motion));
        break;
    }
}
}

class CWindow::Impl
{
public:
    void Show(const std::string &title, const glm::ivec2 &size)
    {
		m_size = size;

		CUtils::InitOnceSDL2();

        // Выбираем Compatiblity Profile
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

        // Специальное значение SDL_WINDOWPOS_CENTERED вместо x и y заставит SDL2
        // разместить окно в центре монитора по осям x и y.
        // Для использования OpenGL вы ДОЛЖНЫ указать флаг SDL_WINDOW_OPENGL.
        m_pWindow.reset(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));

        // Создаём контекст OpenGL, связанный с окном.
		m_pGLContext.reset(SDL_GL_CreateContext(m_pWindow.get()));
		if (!m_pGLContext)
		{
			CUtils::ValidateSDL2Errors();
		}
		CUtils::InitOnceGLEW();
    }

    glm::ivec2 GetWindowSize() const
    {
        return m_size;
    }

    void SetClient(IWindowClient *pClient)
    {
        m_pClient = pClient;
    }

    void DoMainLoop()
	{
		const std::chrono::milliseconds FRAME_PERIOD(16);

        SDL_Event event;
        CChronometer chronometer;
        while (true)
        {
            while (SDL_PollEvent(&event) != 0)
            {
                if (!ConsumeEvent(event) && m_pClient)
                {
                    DispatchEvent(event, *m_pClient);
                }
            }
            if (m_isTerminated)
            {
                break;
            }
            // Очистка буфера кадра, обновление и рисование сцены, вывод буфера кадра.
            Clear();
            if (m_pClient)
            {
                const float deltaSeconds = chronometer.GrabDeltaTime();
                m_pClient->OnUpdateWindow(deltaSeconds);
            }
			CUtils::ValidateOpenGLErrors();
			SwapBuffers();
			chronometer.WaitNextFrameTime(FRAME_PERIOD);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SwapBuffers()const
    {
        // Вывод нарисованного кадра в окно на экране.
        // При этом система отдаёт старый буфер для рисования нового кадра.
        // Обмен двух буферов вместо создания новых позволяет не тратить ресурсы впустую.
        SDL_GL_SwapWindow(m_pWindow.get());
    }

    bool ConsumeEvent(const SDL_Event &event)
    {
        bool consumed = false;
        if (event.type == SDL_QUIT)
        {
            m_isTerminated = true;
            consumed = true;
        }
        else if (event.type == SDL_WINDOWEVENT)
        {
            OnWindowEvent(event.window);
            consumed = true;
        }
        return consumed;
    }

private:
    void OnWindowEvent(const SDL_WindowEvent &event)
    {
        if (event.event == SDL_WINDOWEVENT_RESIZED)
        {
            m_size = {event.data1, event.data2};
        }
    }

    IWindowClient *m_pClient = nullptr;
    SDLWindowPtr m_pWindow;
    SDLGLContextPtr m_pGLContext;
    glm::ivec2 m_size;
    glm::vec4 m_clearColor;
    bool m_isTerminated = false;
};

CWindow::CWindow()
    : m_pImpl(new Impl)
{
}

CWindow::~CWindow()
{
}

void CWindow::Show(const std::string &title, const glm::ivec2 &size)
{
    m_pImpl->Show(title, size);
}

void CWindow::SetBackgroundColor(const glm::vec4 &color)
{
    m_pImpl->SetBackgroundColor(color);
}

void CWindow::SetClient(IWindowClient *pClient)
{
    m_pImpl->SetClient(pClient);
}

glm::ivec2 CWindow::GetWindowSize() const
{
    return m_pImpl->GetWindowSize();
}

void CWindow::DoMainLoop()
{
    m_pImpl->DoMainLoop();
}
