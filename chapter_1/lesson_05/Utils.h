#pragma once
#include <SDL2/SDL_video.h>

namespace detail
{
struct SDLWindowDeleter
{
	void operator()(SDL_Window *ptr)
	{
		SDL_DestroyWindow(ptr);
	}
};
struct SDLGLContextDeleter
{
	void operator()(SDL_GLContext ptr)
	{
		SDL_GL_DeleteContext(ptr);
	}
};
}

// Используем unique_ptr с явно заданным функтором удаления вместо delete.
using SDLWindowPtr = std::unique_ptr<SDL_Window, detail::SDLWindowDeleter>;

// Используем unique_ptr с явно заданным функтором удаления вместо delete.
using SDLGLContextPtr = std::unique_ptr<void, detail::SDLGLContextDeleter>;

class CUtils
{
public:
	CUtils() = delete;

	static void InitOnceSDL2();
	static void InitOnceGLEW();
	static void ValidateSDL2Errors();
	static void ValidateOpenGLErrors();
};

class CChronometer
{
public:
	CChronometer();
	float GrabDeltaTime();

	void WaitNextFrameTime(const std::chrono::milliseconds &framePeriod);

private:
	std::chrono::system_clock::time_point m_lastTime;
};
