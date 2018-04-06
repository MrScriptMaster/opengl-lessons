/*
 * Шейдеры
 * ----------------------
 * 
 * В упражнении 02-simple-drawing мы бегло рассмотрели шейдеры. В этом упражнении мы остановимся на них более подробно.
 * 
 * Шейдер - это программа, способная исполняться на GPU. Шейдер является изолированной программой и не может сообщаться ни с какой
 * из частей конвейера OpenGL прямо. Единственная связь шейдера с внешним миром происходит через его входы и выходы.
 * 
 * Шейдеры пишутся на языке GLSL, который отдаленно напоминает ANSI С. На текущий момент у GLSL есть много версий спецификаций,
 * но ряд источников рекомендует начать знакомство с шейдерами начиная с версии не ниже 3. В этом и последующих примерах
 * мы будем использовать шейдеры 3.3.0.
 * 
 * В упражнении 02-simple-drawing вершинный и фрагментный шейдеры были написаны в том же исходном файле, что и основная программа,
 * но на практике так никто не делает, потому что это нарушает принцип модульности в проекте. Вы можете захотеть заменить шейдеры старой версии
 * на шейдеры более новой версии спецификации, а для этого вам придется править файл с основной программой и к тому же вручную
 * проставлять управляющие последовательности внутри строки. На практике шейдеры оформляются отдельными исходными файлами, которые
 * переписываются программой в строку и добавляются в программу по обычной схеме, как это было в упражнении. В катлоге include вы
 * найдете реализацию класса Shader, который позволяет облегчить внедрение шейдеров в основной цикл рисования.
 * 
 * Далее мы будем подключать шейдеры из отдельных исходных файлов, для именования которых будет использоваться шаблон
 * 
 *   [major].[minor].[name].[vs|fs|gs].glsl
 * 
 * [major].[minor] - мажорный и минорный номера версии шейдера
 * [name] - имя
 * [vs|fs|gs] - тип шейдера: vs - вершинный; fs - фрагментный; gs - геометрический
 * 
 * Например: 3.3.shader.vs.glsl
 * 
 * Все используемые шейдеры лежат в каталоге shaders. Откройте файл 3.3.dummy.vs.glsl
 */

/*
 * Вершинный шейдер
 * --------------------------
 * Вершинный шейдер получает на свой вход массив атрибутов. Программист сам определяет какие атрибуты за что отвечают. OpenGL
 * гарантирует по меньшей мере 16 4-х компонентных атрибутов, которые вы можете записать за один раз. Некоторые аппаратные платформы
 * могут поддерживать и больше.
 * 
 * В этом примере мы покажем как можно за один раз записать в буфер разные атрибуты (координаты вершин и цвета для них). 
 * До этого мы заполняли буфер только одним типом атрибутов - это координаты вершины. Мы будем передавать в вершинный 
 * шейдер одновременно и координаты вершин и цвет для каждой из них. Далее вершинный
 * шейдер вместе с фрагментным объявят разделяемую переменную, в которую вершинный шейдер будет вкладывать цвет, а фрагментный шейдер
 * будет его забирать.
 */

#include "application.h"
#include "shader.h"         // используйте этот класс, чтобы быстро подключать шейдеры к вашей программе

BEGIN_APP_DECLARATION(Shaders)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
protected:
    Shader* m_Shaders;
    unsigned int VBO, VAO;
END_APP_DECLARATION()

DEFINE_APP(Shaders, "Shaders")

#define SHADER_PATH_PREFIX "../shaders"

void Shaders::gInit(const char* title) {
    base::gInit(title);
//----------------------------
    m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader03.vs.glsl", SHADER_PATH_PREFIX"/3.3.shader03.fs.glsl");
    if (!m_Shaders) {
        throw std::logic_error("something wrong with shaders");
    }
    
    float vertices[] = {
    // positions             // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    /*
     * В этом примере атрибуты-позиции и атрибуты-цвета хранятся в одном массиве. На каждый атрибут мы должны заготовить
     * по своему указателю.
     */
    // указатель для атрибута позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // указатель для атрибута цвета
    /*
     * Обратите внимание, что атрибуты, отвечающие за цвет, начинаются со смещением на каждой строке,
     * поэтому последний аргумент в данном случае не нулевой.
     */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
} // gInit

void Shaders::gRender(bool auto_redraw) {
    
     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT);
     m_Shaders->use();
     glBindVertexArray(VAO);
     glDrawArrays(GL_TRIANGLES, 0, 3);
    
    base::gRender(auto_redraw);
} // gRender

void Shaders::gFinalize() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (m_Shaders)
        delete m_Shaders;
    
    base::gFinalize();
} // gFinalize

void Shaders::onKey(int key, int scancode, int action, int mods) {
    
    
    //base::onKey(key,scancode,action,mods);
} // onKey

