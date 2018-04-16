
/* 
 * Здесь хранится стандартная реализация для класса
 * Application.
 */
 
#include "application.h"

/* 
 * void Application::run() реализован в заголовке
 */

//---------------

#define G_DEFAULT_WIN_WIDTH_  800               // default width of main window
#define G_DEFAULT_WIN_HEIGHT_ 600               // default height of main window
#define G_DEFAULT_WIN_TITLE   "OpenGL Application"


//-------- CALLBACKS ---------------------------------------------------
void Application::window_resize_callback(GLFWwindow* window, int width, int height) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->gResize(width, height);
} // window_resize_callback

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->onKey(key, scancode, action, mods);
} // key_callback

void Application::char_callback(GLFWwindow* window, unsigned int codepoint) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->onChar(codepoint);
} // char_callback

void Application::error_callback(int error, const char* desc) {
    std::cerr << "GLFW error: " << desc << std::endl;
} // error_callback

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->onMouseMove(xpos, ypos);
} // mouse_callback

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* pThis = (Application*)glfwGetWindowUserPointer(window);
    pThis->onMouseScroll(xoffset, yoffset);
} // scroll_callback
//-----------------------------------------------------------------------
void Application::gInit(const char* title) {
    if (!glfwInit()) {
        throw std::logic_error("GLFW init error");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, G_CONTEXT_VERSION_MAJOR_);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, G_CONTEXT_VERSION_MINOR_);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // make app window
    m_pWindow = glfwCreateWindow(G_DEFAULT_WIN_WIDTH_, G_DEFAULT_WIN_HEIGHT_, title ? title : G_DEFAULT_WIN_TITLE, nullptr, nullptr);
    if (!m_pWindow) {
        throw std::logic_error("can't create the main window");
    }
    
    // default settings
    glfwSetWindowUserPointer(m_pWindow, this);
    
    /*** callbacks ***/
    
    glfwSetWindowSizeCallback(m_pWindow, window_resize_callback);
    glfwSetKeyCallback(m_pWindow, key_callback);
    glfwSetCharCallback(m_pWindow, char_callback);
    glfwSetErrorCallback(error_callback);
    glfwSetCursorPosCallback(m_pWindow, mouse_callback);
    glfwSetScrollCallback(m_pWindow, scroll_callback);
    
    /*****************/
    glfwMakeContextCurrent(m_pWindow);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::logic_error("error response from the GLAD: failure when GL loading");
    }
    if (m_imain_window_width <=0 || m_imain_window_height <=0 ) {
        gResize(G_DEFAULT_WIN_WIDTH_, G_DEFAULT_WIN_HEIGHT_);
    }
    else {
        gResize(m_imain_window_width, m_imain_window_height);
    }
} // gInit

void Application::gFinalize() {
    if (m_pWindow) {
        glfwDestroyWindow(m_pWindow);
    }
    glfwTerminate();
} // gFinalize
