/*
 * Пользуясь предыдущим примером попробуем нарисовать на сцене несколько ящиков, но в разных позициях на сцене.
 * Так как модели всех ящиков у нас будут абсолютно одинаковые, не нужно делать какие либо изменения в загрузке
 * модели. Все, что мы должны сделать, это применить свою матрицу модели для каждого из ящиков. При этом мы применяем
 * функцию рисования для одних и тех же вершин.
 */ 

#include "application.h"
#include "shader.h"
#include <SOIL/SOIL.h>
#include "model_cube.h"    // вершины для куба мы берем здесь

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

BEGIN_APP_DECLARATION(Cube)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
    Cube() 
    : base(true),
    m_Shaders(nullptr),
    rotate_angle(0.0f)
    {}
protected:
    Shader* m_Shaders;
    GLuint VBO, VAO;
    GLuint texture_box;
    GLfloat rotate_angle;
END_APP_DECLARATION()

DEFINE_APP(Cube, "Cubes")

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(models::cube_vertices), models::cube_vertices, GL_STATIC_DRAW);
    
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
    /* 
     * Много ящиков
     * ---------------------------
     * Мы немного усложним рисование сцены. Все ящики будут рисоваться в цикле. Один из ящиков можно
     * будет вращать, используя стрелки "влево" и "вправо".
     */    
    // Мы задаем разные оси вращения и позиции на сцене через следующий массив векторов
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-0.5f, -0.2f, -1.5f),  
        glm::vec3(-2.8f, -0.0f, -2.3f),  
        glm::vec3( 2.4f, -0.4f, -1.5f),  
        glm::vec3(-1.7f,  1.0f, -4.5f),  
        glm::vec3( 0.3f, -2.0f, -1.5f),  
        glm::vec3( 0.5f,  2.0f, -2.5f), 
        glm::vec3( 0.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    // матрицы
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    // получаем позиции в шейдере
    GLint modelLoc = glGetUniformLocation(m_Shaders->ID, "model");
    GLint viewLoc = glGetUniformLocation(m_Shaders->ID, "view");
    GLint projLoc = glGetUniformLocation(m_Shaders->ID, "projection");
    // матрицы вида и проекции не изменяются
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(45.0f, (GLfloat)800 / (GLfloat)600, 0.1f, 100.0f);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Рисование
    glBindVertexArray(VAO);
    for (uint i = 0; i < sizeof cubePositions / sizeof *cubePositions; i++) {
        // каждый ящик находится на своей позиции
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle;
        if (i == 0)
            angle = rotate_angle;
        else
            angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.3f, 1.0f, 0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
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
        rotate_angle -= 0.7f;
        if (rotate_angle < 0.0f)
            rotate_angle = 360.0f;
    }
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        rotate_angle += 0.7f;
        if (rotate_angle > 360.0f)
            rotate_angle = 0.0f;
    }
} // onKey
