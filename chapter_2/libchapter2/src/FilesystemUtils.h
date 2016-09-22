#pragma once
#include <string>
#include <boost/filesystem/path.hpp>

class CFilesystemUtils
{
public:
    CFilesystemUtils() = delete;

    static boost::filesystem::path GetResourceAbspath(const boost::filesystem::path &path);
    static std::string LoadFileAsString(const boost::filesystem::path &path);
};
