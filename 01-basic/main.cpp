
/*
 * В этом примере показан каркас приложения, использующего GLFW 3 для создания окна и контекста. 
 * Для примера окно заливается сплошным цветом.
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef GLFW_TRUE
#define GLFW_TRUE 1
#endif

#ifndef GLFW_FALSE
#define GLFW_FALSE 0
#endif

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Callbacks
static void error_callback(int error, const char* desc) {
    std::cerr << "Error: " << desc << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::cout << key << " " << scancode << " " << action << " " << mods << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// Main function
int main(int argc, char** argv) {
    
    // Setting error callback
    glfwSetErrorCallback(error_callback);
    
    // Init GLFW
    if (!glfwInit()) {
        std::cout << "Fatal error: GLFW can't init" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create window with context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Hello, World!",NULL,NULL);
    if (!window) {
        std::cout << "Fatal error: Can't create window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // Setting key callback
    glfwSetKeyCallback(window, key_callback);
    
    // Make current context
    glfwMakeContextCurrent(window);
    // Set callback for frame buffer
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
       
    // Init GLAD - set of functions for working with OpenGL pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Fatal error: Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate(); 
        exit(EXIT_FAILURE);
    }  
    
    // Setting swap interval
    glfwSwapInterval(1);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        
        // OpenGL code
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Swap buffer
        glfwSwapBuffers(window);
        // Process events
        glfwPollEvents();
    }
    
    // Destroy window
    glfwDestroyWindow(window);
        
    // Terminate GLFW
    glfwTerminate();    
    std::cout << "Info: GLFW terminated" << std::endl;
    exit(EXIT_SUCCESS);
}
