
#ifndef _USING_GL_INCLUDED_H_
#define _USING_GL_INCLUDED_H_

// Using GLFW3

#include "glad/glad.h"
#include <GLFW/glfw3.h>

// Для указания с помощью функций типа hint версии OpenGL контекста
#define G_CONTEXT_VERSION_MAJOR_  3
#define G_CONTEXT_VERSION_MINOR_  3

#ifndef GLFW_TRUE
#define GLFW_TRUE 1
#endif

#ifndef GLFW_FALSE
#define GLFW_FALSE 0
#endif

#endif
