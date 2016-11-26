#pragma once

enum class ExtensionId
{
    ARB_debug_output,
};

class CPlatformFeatures
{
public:
    static bool HasExtension(ExtensionId id);
};
