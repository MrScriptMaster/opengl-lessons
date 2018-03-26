 
/*
 *  В этом примере демонстрируется, как используется класс Application для быстрого
 *  создания приложения OpenGL
 */

#include "application.h"        // здесь лежит класс

// Объявляем потомка класса Application
// Между макросами перечислите виртуальные 
BEGIN_APP_DECLARATION(Example)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void onKey(int key, int scancode, int action, int mods);
END_APP_DECLARATION()
// Потомок объявлен

/*
 * Объявляем функцию main. Реализация main представлена в заголовке application.h
 * макросом.
 * 
 * Первым аргументом макроса является имя класса потомка Application, который
 * вы объявили ранее. Вторым аргументом передается заголовок окна
 */

DEFINE_APP(Example, "OpenGL Application on C++")

/*
 * Вот и все. Ниже этой строки определите реализацию выбранных виртуальных функций
 */

//--------------------------------------------------------------------------------

void Example::gInit(const char* title) {
    // в стандартной реализации реализовано большинство операций, которые вам
    // понадобятся, поэтому вызывайте ее первой
    base::gInit(title);
    glfwSwapInterval(1);
} // gInit

void Example::gRender(bool auto_redraw) {
    // здесь вы реализуете свое рисование
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // базовая реализация делает swap буферов
    base::gRender(auto_redraw);
}  // gRender

/*
 * gFinalize нужно переопределять, когда вы заняли ресурсы в gInit. Если это так,
 * то всегда вызывайте базовую реализацию gFinalize в конце - это важно!
 * 
 * Здесь мы не переопределяем gFinalize.
 */

/*
 * Если необходимо сделать привязку клавиш к некоторому действию, то переопределите
 * обработчик onKey. По умолчанию он ничего не делает.
 */

void Example::onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(m_pWindow, GLFW_TRUE);
} // onKey
