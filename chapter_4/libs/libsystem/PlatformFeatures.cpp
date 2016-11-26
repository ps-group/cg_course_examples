#include "PlatformFeatures.h"
#include "includes/opengl-common.hpp"

bool CPlatformFeatures::HasExtension(ExtensionId id)
{
#if defined(_WIN32)
    // На Windows используем GLEW.
    switch (id)
    {
    case ExtensionId::ARB_debug_output:
        return (GLEW_ARB_debug_output == GL_TRUE);
    }

#else
    // На остальных платфомах проверяем макросы.
    switch (id)
    {
    case ExtensionId::ARB_debug_output:
        return GL_ARB_debug_output;
    }

#endif
    return false;
}
