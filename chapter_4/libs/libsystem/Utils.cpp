#include "Utils.h"
#include "Window.h"
#include "includes/opengl-common.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include <cstdlib> // для std::memcpy

using namespace std::chrono;

// Вызывает инициализацию SDL2 только один раз за время работы приложения.
// (в многопоточной среде нам потребуется std::call_once вместо static bool)
void CUtils::InitOnceSDL2()
{
	static bool didInit = false;
	if (!didInit)
	{
		const int status = SDL_Init(SDL_INIT_EVERYTHING);
		if (status != 0)
		{
			ValidateSDL2Errors();
		}
	}
}

void CUtils::InitOnceGLEW()
{
    // Инициализируем GLEW только на Windows.
#if defined(_WIN32)
	static bool didInit = false;
	if (!didInit)
	{
		glewExperimental = GL_TRUE;
		GLenum status = glewInit();
		if (status != GLEW_OK)
		{
            const std::string errorStr = reinterpret_cast<const char *>(glewGetErrorString(status));
            throw std::runtime_error("GLEW initialization failed: "
                                     + errorStr);
		}

        // GLEW при инициализации использует вызов glGetString,
        //   недопустимый для Core Profile.
        // Вызываем glGetError(), чтобы очистить буфер ошибки OpenGL.
        //   http://stackoverflow.com/questions/10857335/
        glGetError();
	}
#endif
}

void CUtils::ValidateSDL2Errors()
{
	std::string message = SDL_GetError();
	if (!message.empty())
    {
        throw std::runtime_error("SDL2 error: " + message);
	}
}

void CUtils::ValidateOpenGLErrors()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::string message;
		switch (error)
		{
		case GL_INVALID_ENUM:
			message = "invalid enum passed to GL function (GL_INVALID_ENUM)";
			break;
		case GL_INVALID_VALUE:
			message = "invalid parameter passed to GL function (GL_INVALID_VALUE)";
			break;
		case GL_INVALID_OPERATION:
			message = "cannot execute some of GL functions in current state (GL_INVALID_OPERATION)";
			break;
		case GL_STACK_OVERFLOW:
			message = "matrix stack overflow occured inside GL (GL_STACK_OVERFLOW)";
			break;
		case GL_STACK_UNDERFLOW:
			message = "matrix stack underflow occured inside GL (GL_STACK_UNDERFLOW)";
			break;
		case GL_OUT_OF_MEMORY:
			message = "no enough memory to execute GL function (GL_OUT_OF_MEMORY)";
			break;
		default:
			message = "error in some GL extension (framebuffers, shaders, etc)";
			break;
		}
        throw std::runtime_error("OpenGL error: " + message);
	}
}

void CUtils::FlipSurfaceVertically(SDL_Surface &surface)
{
    const auto rowSize = size_t(surface.w * surface.format->BytesPerPixel);
    std::vector<uint8_t> row(rowSize);

    // Зеркально отражаем пиксели по оси Y,
    //  если число строк пикселей в изображении нечётное,
    //  центральная строка остаётся нетронутой.
    for (size_t y = 0, height = size_t(surface.h); y < height / 2; ++y)
    {
        auto *pixels = reinterpret_cast<uint8_t*>(surface.pixels);
        auto *upperRow = pixels + rowSize * y;
        auto *lowerRow = pixels + rowSize * (height - y - 1);
        std::memcpy(row.data(), upperRow, rowSize);
        std::memcpy(upperRow, lowerRow, rowSize);
        std::memcpy(lowerRow, row.data(), rowSize);
    }
}

SDLSurfacePtr CUtils::RenderUtf8Text(TTF_Font &font, const std::string &text, const glm::vec3 &color)
{
    using namespace glm;

    const vec3 scaledColor = 255.f * clamp(color, vec3(0.f), vec3(1.f));
    SDL_Color rgbaColor;
    rgbaColor.r = Uint8(scaledColor.r);
    rgbaColor.g = Uint8(scaledColor.g);
    rgbaColor.b = Uint8(scaledColor.b);
    rgbaColor.a = 255;

    return SDLSurfacePtr(TTF_RenderUTF8_Blended(&font, text.c_str(), rgbaColor));
}

CChronometer::CChronometer()
	: m_lastTime(system_clock::now())
{
}

float CChronometer::GrabDeltaTime()
{
    auto newTime = system_clock::now();
    auto timePassed = duration_cast<milliseconds>(newTime - m_lastTime);
    m_lastTime = newTime;
    return 0.001f * float(timePassed.count());
};

void CChronometer::WaitNextFrameTime(const milliseconds &framePeriod)
{
    system_clock::time_point nextFrameTime = m_lastTime + framePeriod;
    std::this_thread::sleep_until(nextFrameTime);
}
