/*
 * В этом примере демонстрируется как работать с uniform объектами внутри
 * шейдера. Мы изменяем цвет в каждом цикле рисования, создавая простейшую
 * анимацию. В данном случае зеленый треугольник плавно изменяет свой цвет 
 * от зеленного до черного.
 */

#include "application.h"
#include "shader.h"

#include <cmath>

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
    
    if (!(m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader04.vs.glsl", 
                                 SHADER_PATH_PREFIX"/3.3.shader04.fs.glsl"))) {
        throw std::logic_error("something wrong with shaders");
    }
    
    float vertices[] = {
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f   // top 
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    /*
     * У нас всего один VAO буфер - один объект для рисования - поэтому мы можем
     * его привязать к контексту здесь и не перепривязывать в основном цикле рисования.
     */
    glBindVertexArray(VAO);
    
} // gInit

void Shaders::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_Shaders->use();
    /*
     * В этом примере, внутри фрагментного шейдера объявлен uniform
     * объект. Мы вычисляем цвет в основном цикле и записываем результат
     * в этот uniform объект.
     */
     float timeValue = glfwGetTime();
     float greenValue = sin(timeValue) / 2.0f + 0.5f;
     //int vertexColorLocation = glGetUniformLocation(m_Shaders->ID, "ourColor");
     //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
     //
     // !!! Этот вызов заменяет первые два
     //
     m_Shaders->setVec4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);
     
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
    
    
} // onKey

