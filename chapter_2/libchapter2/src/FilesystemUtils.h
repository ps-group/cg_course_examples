#pragma once
#include "Utils.h"
#include "filesystem_alias.h"
#include <string>

class CFilesystemUtils
{
public:
	CFilesystemUtils() = delete;

	static fs::path GetResourceAbspath(const fs::path& path);
	static std::string LoadFileAsString(const fs::path& path);
	static SDLSurfacePtr LoadImageByPath(const fs::path& path);
	static TTFFontPtr LoadFixedSizeFont(const fs::path& path, int pointSize);
};
