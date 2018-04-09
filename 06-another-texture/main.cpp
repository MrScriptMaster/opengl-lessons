 
/*
 * В этом примере демонстрируется текстурный блок. Текстурный блок - это прием наложения одновременно нескольких
 * текстур на одну поверхность. Это достигается путем объявления нескольих sampler'ов во фрагментном шейдере.
 */
 
#include "application.h"
#include "shader.h"
#include "SOIL.h"

#include <iostream>

BEGIN_APP_DECLARATION(Textures)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
protected:
    Shader* m_Shaders;
    GLuint VBO, VAO, EBO;
    GLuint texture_1, texture_2;
END_APP_DECLARATION()

DEFINE_APP(Textures, "Another Textures")

#define SHADER_PATH_PREFIX    "../shaders"
#define TEXTURE_PATH_PREFIX   "../textures"

void Textures::gInit(const char* title) {
    base::gInit(title);
    
    if (!(m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader06.vs.glsl", 
                                 SHADER_PATH_PREFIX"/3.3.shader06.fs.glsl"))) {
        throw std::logic_error("something wrong with shaders");
    }
    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
   
    int width, height;
    unsigned char *data;
    /*
     * Для загрузки изображений для текстур мы используем библиотеку SOIL
     * http://www.lonesock.net/soil.html
     */
    //------------------------------
    // Загрузка первой текстуры
    //------------------------------
    glGenTextures(1, &texture_1);
    glBindTexture(GL_TEXTURE_2D, texture_1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = SOIL_load_image(TEXTURE_PATH_PREFIX"/wood.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "tex 1 - success" << std::endl;
    }
    else {
        std::cout << "Failed loading of the texture 1" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);
    //------------------------------
    // Загрузка второй текстуры
    //------------------------------
    glGenTextures(1, &texture_2);
    glBindTexture(GL_TEXTURE_2D, texture_2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = SOIL_load_image(TEXTURE_PATH_PREFIX"/attention_label.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "tex 2 - success" << std::endl;
    }
    else {
        std::cout << "Failed loading of the texture 2" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);   
} // gInit

void Textures::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_Shaders->use();
    
    // Рисование текстурного блока
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_1);
    m_Shaders->setInt("ourTexture1",0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_2);
    m_Shaders->setInt("ourTexture2",1);
    
    // Рисование
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    base::gRender(auto_redraw);
} // gRender

void Textures::gFinalize() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    if (m_Shaders)
        delete m_Shaders;
    base::gFinalize();
} // gFinalize

void Textures::onKey(int key, int scancode, int action, int mods) {
    
    
} // onKey
