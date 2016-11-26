#pragma once

// На платформе Windows без видеодрайвера доступен только OpenGL 1.1,
//  и для получения более новых версий мы применим библиотеку GLEW,
//  заголовок которой должен быть подключён перед `gl.h`
// См. также http://stackoverflow.com/questions/3933027
#if defined(_WIN32)
#include <GL/glew.h>
#include <GL/gl.h>
#endif

// На платформе MacOSX мы не нуждаемся в подключении расширений OpenGL,
//  версия OpenGL определяется целевой версией ОС.
// См. также http://stackoverflow.com/questions/27933009/
#if defined(__APPLE__)
#include <GL/gl.h>
#endif

// На платфоме GNU/Linux для подключения расширений OpenGL не требуется GLEW,
//  но следует объявить макрос GL_GLEXT_PROTOTYPES перед включением заголовков.
#if defined(__linux__)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

