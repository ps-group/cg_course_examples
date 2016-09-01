#pragma once
#include <string>

class CFilesystemUtils
{
public:
    CFilesystemUtils() = delete;

    static std::string GetResourceAbspath(const std::string &path);
    static std::string LoadFileAsString(const std::string &path);
};
