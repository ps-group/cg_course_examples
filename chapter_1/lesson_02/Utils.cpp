#include "stdafx.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <iostream>

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
	static bool didInit = false;
	if (!didInit)
	{
		glewExperimental = GL_TRUE;
		GLenum status = glewInit();
		if (status != GLEW_OK)
		{
			std::cerr << "GLEW initialization failed: " << glewGetErrorString(status) << std::endl;
			std::abort();
		}
	}
}

void CUtils::ValidateSDL2Errors()
{
	std::string message = SDL_GetError();
	if (!message.empty())
	{
		std::cerr << "SDL2 error: " << message << std::endl;
		std::abort();
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
		std::cerr << "OpenGL error: " << message << std::endl;
		std::abort();
	}
}

CChronometer::CChronometer()
	: m_lastTime(std::chrono::system_clock::now())
{
}

float CChronometer::GrabDeltaTime()
{
	auto newTime = std::chrono::system_clock::now();
	auto timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - m_lastTime);
	m_lastTime = newTime;
	return 0.001f * float(timePassed.count());
};
