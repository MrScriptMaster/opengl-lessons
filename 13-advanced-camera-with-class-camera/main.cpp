/*
 * Этот пример построен из 13-го примера, но использует класс Camera.
 */

#include "application.h"
#include "shader.h"
#include "SOIL.h"
#include "model_cube.h"
#include "camera.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

BEGIN_APP_DECLARATION(Cube)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
    void onMouseMove(double xpos, double ypos);
    void onMouseScroll(double xoffset, double yoffset);
    Cube() 
    : base(),
    m_Shaders(nullptr)
    {}
protected:
    Shader* m_Shaders;
    //Camera  m_Camera; 
    GLuint VBO, VAO;
    GLuint texture_box;
END_APP_DECLARATION()

DEFINE_APP(Cube, "Cubes")

#define SHADER_PATH_PREFIX    "../shaders"
#define TEXTURE_PATH_PREFIX   "../textures"

//----------------------------------------------------------------------------
// Настройка камеры
Camera m_Camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX =  800.0f / 2.0;    // позиция курсора мыши по горизонтали
float lastY =  600.0f / 2.0;    // позиция курсора мыши по вертикали

// timing
float deltaTime = 0.0f;         // разница между отрисовкой кадров
float lastFrame = 0.0f;         // момент отрисовки последнего кадра
//----------------------------------------------------------------------------
void Cube::gInit(const char* title) {
    base::gInit(title);

    // Отключаем курсор
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
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
    //Обновляем разницу между кадрами
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    //------------------------------------------------------------    
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
    glm::mat4 view;         
    glm::mat4 projection;
     
    // настройка камеры
    view = m_Camera.GetViewMatrix();
    m_Shaders->setMat4("view", view);
    
    // настройка проекции
    projection = glm::perspective(glm::radians(m_Camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);
    m_Shaders->setMat4("projection", projection);
    
    // Рисование
    glBindVertexArray(VAO);
    for (uint i = 0; i < sizeof cubePositions / sizeof *cubePositions; i++) {
        // каждый ящик находится на своей позиции
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.3f, 1.0f, 0.5f));
        m_Shaders->setMat4("model", model);
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
    float m_CameraSpeed = 2.5 * deltaTime;
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_Camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_Camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_Camera.ProcessKeyboard(LEFT, deltaTime);
    else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_Camera.ProcessKeyboard(RIGHT, deltaTime);
} // onKey

//---------------------------------------------------------------------
void Cube::onMouseMove(double xpos, double ypos) {
     if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    m_Camera.ProcessMouseMovement(xoffset, yoffset);
} // mouse_callback

void Cube::onMouseScroll(double xoffset, double yoffset) {
    m_Camera.ProcessMouseScroll(yoffset);
} // scroll_callback
