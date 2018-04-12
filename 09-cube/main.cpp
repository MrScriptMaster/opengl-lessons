/*
 * Теперь у нас есть все знания, чтобы визуализировать трехмерный объект. В этом примере это будет куб. После наложения текстуры
 * куб будет довольно реалистичным "деревянным ящиком".
 * Чтобы построить куб необходимо иметь 36 точек (каждая сторона строится по двум треугольникам и у каждого по три вершины).
 * В этом примере вершины хранятся в самой программе, но это сделано только в академических целях. На практике
 * вершины объектов хранятся в файлах в некотором формате, которые считываются и выгружаются основной программой
 * по мере необходимости.
 * 
 * Также мы познакомимся с z-буфером (или буфер глубины), который используется в тестах на глубину. Он нам будет нужен, чтобы создать иллюзию
 * глубины сцены.
 */ 

#include "application.h"
#include "shader.h"
#include "SOIL.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
 * Атрибуты для модели куба
 * 
 * В этой модели мы отказываемся от атрибутов цветов вершин, потому что мы все равно накладываем
 * текстуру. Это позволяет сэкономить немного памяти, потому что массив становится меньше.
 */
GLfloat cube_vertices[] = {
    // координаты         // текстурные координаты
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

BEGIN_APP_DECLARATION(Cube)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
    Cube() 
    : base(),
    m_Shaders(nullptr),
    rotate_angle(0.0f)
    {}
protected:
    Shader* m_Shaders;
    GLuint VBO, VAO;
    GLuint texture_box;
    GLfloat rotate_angle;
END_APP_DECLARATION()

DEFINE_APP(Cube, "Cube")

#define SHADER_PATH_PREFIX    "../shaders"
#define TEXTURE_PATH_PREFIX   "../textures"

void Cube::gInit(const char* title) {
    base::gInit(title);
    
    if (!(m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader09.vs.glsl", 
                                 SHADER_PATH_PREFIX"/3.3.shader05.fs.glsl"))) {
        throw std::logic_error("something wrong with shaders");
    }
    //---------------------------
    // Загрузка модели
    //---------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    //---------------------------
    // Загрузка текстуры
    //---------------------------
    int width, height;
    unsigned char *data;
    glGenTextures(1, &texture_box);
    glBindTexture(GL_TEXTURE_2D, texture_box);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = SOIL_load_image(TEXTURE_PATH_PREFIX"/box.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed loading of the texture" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);
    
    // Модель коробки готова
    
} // gInit

void Cube::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    /*
     * Буфер глубины
     * -----------------------
     * Каждый фрагмент хранит информацию о себе в буфере глубины. По умолчанию GLFW создает буфер глубины, однако, он находится в выключенном
     * состоянии. Буфер глубины помогает OpenGL понять какой фрагмент ближе, а какой дальше от камеры. При каждой новой отрисовки сцены фрагмент
     * тестируется на глубину, т.е. сравнивается с другими фрагментами, находящимися в той же позиции на экране. Фрагмент, который находится ближе
     * к камере, чем остальные, выводится на экран, а остальные отбрасываются. Этот процесс называется тестом глубины.
     * 
     * На практике при использовании буфера глубины на каждой новой итерации он должен быть предварительно очищен, чтобы OpenGL не использовал
     * результаты предыдущей перерисовки. Также на каждой итерации буфер глубины должен быть активизирован, через вызов glEnable().
     */
    glEnable(GL_DEPTH_TEST);                                // активизируем буфер глубины
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // очищаем буфер цветов и буфер глубины
    m_Shaders->use();
    // подготовка текстуры
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_box);
    // передача текстуры во фрагментный шейдер
    m_Shaders->setInt("ourTexture",0);
    // матрицы 
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    model = glm::rotate(model, glm::radians(rotate_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (GLfloat)800 / (GLfloat)600, 0.1f, 100.0f);
    GLint modelLoc = glGetUniformLocation(m_Shaders->ID, "model");
    GLint viewLoc = glGetUniformLocation(m_Shaders->ID, "view");
    GLint projLoc = glGetUniformLocation(m_Shaders->ID, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); 
    
    // Рисование
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    base::gRender(auto_redraw);
} // gRender

void Cube::gFinalize() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (m_Shaders)
        delete m_Shaders;
    base::gFinalize();
} // gFinalize

void Cube::onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        rotate_angle -= 1.0f;
        if (rotate_angle < 0.0f)
            rotate_angle = 360.0f;
    }
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        rotate_angle += 1.0f;
        if (rotate_angle > 360.0f)
            rotate_angle = 0.0f;
    }
} // onKey

