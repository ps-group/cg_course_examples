#pragma once
#include <string>
#include <boost/filesystem/path.hpp>

class CFilesystemUtils
{
public:
    CFilesystemUtils() = delete;

    static boost::filesystem::path GetResourceAbspath(const std::string &path);
    static std::string LoadFileAsString(const std::string &path);
};
