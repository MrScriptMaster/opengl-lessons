/*
 * Системы координат
 * 
 * До этого примера вы уже должны понимать, что такое объект на сцене, атрибуты объекта, шейдеры, принцип рисования объекта и
 * как накладываются на объект текстуры. Кроме того, вы должны уже иметь представление, как осуществляются трансформации объекта.
 * Здесь мы наконец то рассмотрим объект как часть трехмерной сцены, в частности как наблюдатель видит сцену.
 * 
 * До этого момента мы передавали в шейдер нормализованные координаты, но обычно так на практике не делают. Разработчик сам определяет масштабы
 * сцены. 
 * 
 * На сцене можно выделить 5 систем координат:
 *      1. Локальная система координат. Каждый объект на сцене имеет по собственной локальной системе координат, в которой
 *         происходят преобразования только для его вершин.
 *      2. Мировая система координат - это система координат всей сцены. Наблюдатель может не видеть всей сцены, а видеть только её кусок.
 *      3. Пространство вида (камера). Камера сцены позволяет посмотреть на нее под определенным углом. Наблюдатель
 *         в каждый конкретный момент смотрит на конкретную проекцию и линия его взора перпендикулярна проекции.
 *      4. Пространство отсечения. В пространстве отсечения все координаты находятся в нормированном виде, т.е. в диапазоне от -1.0 до +1.0. Все координаты
 *         которые выходят за этот диапазон после нормировки отсекаются. Также на этом этапе настраивается перспектива.
 *      5. Экранное пространство. Это пространство, ограниченное Viewport. В конечном итоге наблюдатель смотрит на экранное пространство.
 *         Перевод в экранное простраство осуществляет драйвер и его программировать особо не нужно.
 * 
 * Координаты вершин каждого объекта прежде чем попасть в экранное пространство проходят трансформацию именно в том порядке, в котором они перечислены выше.
 * Трансформация производится с помощью матриц. 
 * Наиболее важными матрицами являются следующие:
 *      1. Матрица модели - это матрица, через которую преобразуются координаты из локальной системы в мировую. В задачи этого преобразования входит расположение
 *         всех объектов по сцене.
 *      2. Матрица вида - это матрица, через которую преобразуются координаты из мировой системы в пространство вида. В задачи этого преобразования входит
 *         расположить наблюдателя на сцене.
 *      3. Матрица проекции - это матрица, через которую координаты из пространства вида преобразуются в координаты пространства отсечения. В задачи этой матрицы
 *         входит определение протяженности сцены, т.е. ее масштабы для наблюдателя, и метод проецирования (форма пирамиды отсечения). 
 *         Иными словами этот этап определяет насколько наблюдатель широко видит сцену.
 * 
 * В пространстве отсечения сцена может быть представлена в одной из двух проекций, в зависимости от формы пирамиды отсечения. Если пирамида отсечения представлена прямоугольным
 * параллелограммом, то проекция называется ортографической. Если пирамида отсечения это усеченная пирамида, то говорят о перспективной проекции. Перспективная проекция больше всего похожа на то,
 * что видит глаз человека, когда дальние предметы кажутся меньше ближних. Если небольшой предмет находится очень близко к наблюдателю, то дальние и ближние вершины не создают такой эффект - это пример
 * ортографической проекции. Обычно ортографическое проецирование используется при конструировании моделей, когда конструктору не нужны искажения от перспективы.
 * 
 * Разделение на пространства упрощает задачу программирования динамики и расстановки объектов по сцене. Например, если ваш объект это автомобиль, то анимирование открывания двери разумно
 * проводить в локальном пространстве автомобиля. Когда автомобиль движется по сцене, то его позиционирование разумно производить с точки зрения всей сцены.
 * 
 * Алгоритм преобразования
 * --------------------------------
 * Вершинный шейдер должен отдавать нормализованные координаты. Перемножение матриц необходимо проводить в нем же. Как и при трансформации, рассмотренной в примере 7, 
 * перемножение матриц должно быть в обратном порядке. В общем случае форула получения координат в пространстве отсечения выглядит такой:
 * 
 *          V_clip = M_proj * M_view * M_model * V_local,
 * где
 *      V_clip - координаты в отсеченном пространстве
 *      M_proj - матрица проекции
 *      M_view - матрица вида
 *      M_model - матрица модели
 *      V_local - координаты в локальной системе координат
 * 
 * Координаты V_clip затем просто присваиваются переменной gl_Position, которая встроена в вершинный шейдер. Перспективное деление и отсечение выполняется OpenGL автоматически.
 */

#include "application.h"
#include "shader.h"
#include <SOIL/SOIL.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

BEGIN_APP_DECLARATION(Scene)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
    Scene() 
    : base(true),
    m_Shaders(nullptr)
    {}
protected:
    Shader* m_Shaders;
    GLuint VBO, VAO, EBO;
    GLuint texture_box;
END_APP_DECLARATION()

DEFINE_APP(Scene, "3D Scene")

#define SHADER_PATH_PREFIX    "../shaders"
#define TEXTURE_PATH_PREFIX   "../textures"

void Scene::gInit(const char* title) {
    base::gInit(title);
    if (!(m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader08.vs.glsl", 
                                SHADER_PATH_PREFIX"/3.3.shader05.fs.glsl"))) {
        throw std::logic_error("something wrong with shaders");
    }
    // Деревянный ящик
    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };
    GLuint indices[] = {  
        0, 1, 3, // Первый полигон 
        1, 2, 3  // Второй полигон
    };
    // Загружаем модель в буферы
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Координаты вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Цвета для вершин
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Координаты для текстуры
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); 
    //
    // Модель загружена в буферы
    //
    //=====================================
    // Загрузка текстуры
    //=====================================
    int width, height;
    unsigned char *data = NULL;
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

void Scene::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_Shaders->use();
    // подготовка текстуры
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_box);
    // передача текстуры во фрагментный шейдер
    m_Shaders->setInt("ourTexture",0);
    
    // Подготовка и передача матриц
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    /*
     * Мы двигаем сцену от себя. С точки зрения наблюдателья, мы отходим назад.
     */
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    /*
     * Настройка перспективы.
     */
    projection = glm::perspective(glm::radians(45.0f), (GLfloat)800 / (GLfloat)600, 0.1f, 100.0f);
    //
    // Передача матриц в шейдер
    //
    GLint modelLoc = glGetUniformLocation(m_Shaders->ID, "model");
    GLint viewLoc = glGetUniformLocation(m_Shaders->ID, "view");
    GLint projLoc = glGetUniformLocation(m_Shaders->ID, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); 
    /*
     * Примечание:
     * Обратите внимание на то, что матрицу проекции в этом примере мы передаем каждый раз. Однако, как правило, сцена редко меняет
     * настройку проекции, поэтому зачастую достаточно ее передавать один раз.
     */
    
    // Рисование
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    base::gRender(auto_redraw);
} // gRender

void Scene::gFinalize() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    if (m_Shaders)
        delete m_Shaders;
    base::gFinalize();
} // gFinalize

void Scene::onKey(int key, int scancode, int action, int mods) {
    
} // onKey
