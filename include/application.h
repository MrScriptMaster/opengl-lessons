
/*
 * Болванка для OpenGL приложения
 * v0.9
 * 
 * (Поддерживаются только Unix и Unix-like)
 * 
 * Инструкция по использованию:
 * 
 *  1. В конце заголовка приведены необходимые макросы. Внимательно их просмотрите.
 *  2. В своем приложении с помощью пары макросов BEGIN_APP_DECLARATION и END_APP_DECLARATION
 *     объявите потомка класса Application
 * 
 *  Пример
 * 
 *  BEGIN_APP_DECLARATION(example)
 *      // здесь вы объявляете функции, которые собираетесь переопределить
 *      virtual void gInit(const char* title = NULL);
 *      virtual void gRender(bool auto_redraw = true);
 *      virtual void gFinalize();
 *      virtual void gResize(int width, int height);
 *      void onKey(int key, int scancode, int action, int mods);
 *  END_APP_DECLARATION()
 *  
 *  3. Далее используйте макрос DEFINE_APP(appclass,title), чтобы вставить типовую реализацию
 *     функции main. В большинстве случаев вам менять в функции ничего не нужно, но если это не так,
 *     то посмотрите стандартную реализацию и обязательно включите ее код в свою версию main.
 *     Первым параметром передайте имя класса потока из п.2, а вторым параметром передайте заголовок
 *     для окна.
 * 
 */

#ifndef _APPLICATION_INCLUDED_H_
#define _APPLICATION_INCLUDED_H_

#include "using_gl.h"

#include <iostream>
#include <exception>
#include <stdexcept>

class Application {
protected:
    inline Application() {}
    virtual ~Application() {}
    
    static Application* s_app;
    GLFWwindow* m_pWindow;
    
    // default callbacks
    static void window_resize_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void char_callback(GLFWwindow* window, unsigned int codepoint);
    static void error_callback(int error, const char* desc);
    
public:
    /**
     * Запускает главный цикл приложения
     */
    void run();
    
    /*
     * Замечание: методы, которые начинаются на g, используют функции OpenGL
     */
    
    /**
     * Начальная инициализация всей графики. Должна вызываться до всех функций рисования.
     * Внутри себя ничего не должна рисовать.
     * 
     * /param title - заголовок окна
     * 
     */
    virtual void gInit(const char* title = NULL);
    
    /**
     * Главная функция отрисовки графики.
     * 
     * /param auto_redraw - флаг, сообщающий о том, что сцена
     * должна перерисовываться автоматически с некоторым интервалом
     */
    virtual void gRender(bool auto_redraw = true) {
        glfwSwapBuffers(m_pWindow);
    }
    
    /**
     * Деинициализация. Должна вызываться перед выходом из приложения и после выхода из
     * главного цикла приложения.
     */
    virtual void gFinalize();
    
    /**
     * Позволяет изменить размер Viewport
     */
    virtual void gResize(int width, int height) {
        glViewport(0, 0, width, height);
    }
    
    /**
     * Позволяет привязать функциональность к клавишам клавиатуры.
     */
    virtual void onKey(int key, int scancode, int action, int mods) {}
    /**
     * Обратная реакция для специфичных символов
     */
    virtual void onChar(unsigned int codepoint) {}
    
};  // class Application

/****** Macro definitions ******/
/*
 * BEGIN_APP_DECLARATION(appclass)
 * 
 * Используйте следующий макрос для объявления потомка класса Application. В качестве
 * параметра макроса передается имя для класса-потомка. Далее вы должны переопределить
 * виртуальные функции, которые вам нужны в вашем приложении.
 * 
 * ВАЖНО: этот макрос должен закрываться парным ему END_APP_DECLARATION()
 * 
 */
#define BEGIN_APP_DECLARATION(appclass)                     \
class appclass : public Application                         \
{                                                           \
public:                                                     \
    typedef class Application base;                         \
    static Application * Create(void)                       \
    {                                                       \
        return (s_app = new appclass);                      \
    }

/*
 * END_APP_DECLARATION()
 * 
 * Используйте этот макрос, чтобы закрыть BEGIN_APP_DECLARATION(appclass)
 */
#define END_APP_DECLARATION()                               \
};

/*
 * Определение функции main() (первая строка)
 */
#define MAIN_DECL int main(int argc, char ** argv)

/*
 * DEFINE_APP(appclass,title)
 * 
 * Типовая реализация приложения. Используйте этот макрос, если
 * функциональности этой реализации функции main вам достаточно.
 * 
 *     appclass - имя для класса-потомка Application, экземпляр
 *       которого нужно создать в приложении.
 *     title - заголовок окна приложения.
 *    
 */
#define DEFINE_APP(appclass,title)                                             \
Application * Application::s_app = NULL;                                       \
                                                                               \
void Application::run()                                                        \
{                                                                              \
    do                                                                         \
    {                                                                          \
        gRender();                                                             \
        glfwPollEvents();                                                      \
    } while (!glfwWindowShouldClose(m_pWindow));                               \
}                                                                              \
                                                                               \
MAIN_DECL                                                                      \
{                                                                              \
    int result = 0;                                                            \
    Application * app = appclass::Create();                                    \
    try {                                                                      \
        if (app) {                                                             \
            app->gInit(title);                                                 \
            app->run();                                                        \
        }                                                                      \
        else {                                                                 \
            throw std::logic_error("object of appclass is not created");       \
        }                                                                      \
    }                                                                          \
    catch (const std::exception& e) {                                          \
        std::cout << __FILE__ << " (" << __LINE__ << ")"                       \
                  << ":" << __FUNCTION__                                       \
                  << ": " << e.what() << std::endl;                            \
        result = -1;                                                           \
    }                                                                          \
    catch (...) {                                                              \
        std::cout << __FILE__ << " (" << __LINE__ << ")"                       \
                  << ":" << __FUNCTION__                                       \
                  << ": " << "unknown exception" << std::endl;                 \
        result = -1;                                                           \
    }                                                                          \
    if (app) {                                                                 \
        app->gFinalize();                                                      \
    }                                                                          \
    return result;                                                             \
}

/****** End macro definitions ******/

#endif // _APPLICATION_INCLUDED_H_
 
