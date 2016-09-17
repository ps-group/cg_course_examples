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
		if (0 != SDL_Init(SDL_INIT_EVERYTHING))
		{
			std::cerr << "SDL2 initialization failed: " << SDL_GetError() << std::endl;
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
