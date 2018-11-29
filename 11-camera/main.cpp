/*
 * В этом примере мы разберем управление камерой подробнее.
 * Камера имеет ряд характеристик, которыми мы можем управлять в каждый момент отрисовки:
 *  - Позиция - это координата начала отсчета локальной системы координат камеры относительно
 *    мировой системы координат.
 *  - Направление - ось (обычно это ось z) локальной системы координат камеры, 
 *    проходящая через позицию камеры и ортогональная проекции сцены. Положительное
 *    направление для этой оси обычно направлено в сторону наблюдателя, а отрицательное
 *    направление - в сторону сцены. Когда камера перемещается в положительном направлении
 *    по этой оси, то возникает ощущение, что наблюдатель отходит от сцены пятясь назад.
 *  - Правая ось - обычно это локальная ось x.
 *  - Верхняя ось - обычно это локальная ось y.
 * 
 * Управление камерой основано на вычислении особой матрицы, которая называется Look At ("смотри на").
 * Look At матрица является произведением матрицы составленной из векторов, лежащих на оси направления, правой оси, верхней оси, и
 * матрицы составленной из радиус-вектора позиции камеры и единичной матрицы. Матрица Look At по сути является матрицей вида.
 * В библиотеку GLM уже встроена функция по вычислению Look At матрицы, т.е. вам достаточно задать только характеристики камеры.
 */ 

#include "application.h"
#include "shader.h"
#include <SOIL/SOIL.h>
#include "model_cube.h"

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
    /*
     * Оптимизация
     * Мы можем облегчить цикл перерисовки, если вынесем в инициализацию все компоненты, которые не изменяются
     * во время работы программы.
     */
    m_Shaders->use();
    // настройка проекции
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    GLint projLoc = glGetUniformLocation(m_Shaders->ID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
} // gInit

/* Позиции кубов вынесены в глобальную память
 * Это решение только для академических целей!
 */
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

void Cube::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_Shaders->use();
    // подготовка текстуры
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_box);
    // передача текстуры во фрагментный шейдер
    m_Shaders->setInt("ourTexture",0);

    // матрицы
    glm::mat4 model;
    glm::mat4 view;     // камера
   
    // получаем позиции в шейдере
    GLint modelLoc = glGetUniformLocation(m_Shaders->ID, "model");
    GLint viewLoc = glGetUniformLocation(m_Shaders->ID, "view");
   
    // настройка камеры
    float radius = 10.0f;
    // Наша камера движется по окружности, центр которой совпадает с началом мировой системы координат.
    // Следующая параметрическая система уравнений вычисляет очередную позицию камеры.
    float camX   = sin(glfwGetTime()) * radius;
    float camZ   = cos(glfwGetTime()) * radius;
    /*
     * Расчет матрицы Look At производится одноименным методом
     * Правая ось в данном методе вычисляется по переданным параметрам, потому что
     * в OpenGL используется правая система координат.
     * 
     *  Позиция - радиус вектор в мировой системе координат, на конце которого находится камера.
     *  Направление - вектор, начало которого находится в позиции камеры, а конец - в точке, на которую камера смотрит (эту точку мы передаем).
     *  Верхняя ось - учитывает ориентацию камеры относительно направления. В данном случае она ориентирована по горизонту.
     */
    //                 //позиция                    //направление                //верхняя ось
    view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  
    // Рисование
    glBindVertexArray(VAO);
    for (uint i = 0; i < sizeof cubePositions / sizeof *cubePositions; i++) {
        // каждый ящик находится на своей позиции
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = 20.0f * i;
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

} // onKey
