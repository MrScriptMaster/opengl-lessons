
/* 
 * Здесь хранится стандартная реализация для класса
 * Application.
 */
 
#include "application.h"

/* 
 * void Application::run() реализован в заголовке
 */

//---------------

void Application::window_resize_callback(GLFWwindow* window, int width, int height) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->Resize(width, height);
} // window_resize_callback

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->OnKey(key, scancode, action, mods);
} // key_callback

void Application::gInit(const char* title) {
    if (!glfwInit()) {
        throw std::logic_error("GLFW init error");
    }
} // gInit

void Application::gFinalize() {
    glfwTerminate();
} // gFinalize