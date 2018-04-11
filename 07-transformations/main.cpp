/*
 * До этого объекты, которые мы рисовали, были статичными. Движение объектов на сцене производится через
 * пересчеты координат их вершин. Обычно все движения сводятся к линейному перемещению, вращению и их комбинации.
 * Для расчетов движений необходимо использовать матрицы и векторы, встроенные в OpenGL.
 * 
 * Следует отличать радиус-вектор и произвольный вектор. Радиус-вектор всегда начинается в начале координат и указывает на 
 * некоторую точку в пространстве. Произвольный вектор может начинаться и заканчиваться в любых точках пространства.
 * Радиус-вектор используют, чтобы указать на точку в пространстве (или на вершину), тогда как произвольный вектор обычно используется, чтобы
 * указать на линейную траекторию движения.
 * 
 * На практике используются следующие операции над векторами:
 *    - сложение вектора со скаляром
 *    - инверсия вектора
 *    - сложение вектора с другим вектором
 *    - получение длины вектора
 *    - нормализация векторов
 *    - скалярное произведение (как следствие этой операции, может быть получение угла между векторами)
 *    - векторное произведение (как следствие этой операции, может быть получение нормали)
 * 
 * На практике используют векторы 4 порядка, в которых первые три элемента это координаты в трехмерном пространстве, а
 * четвертный элемент - это гомогенная координата. Только векторы 4-го порядка можно перемещать в трехмерном пространстве.
 * 
 * Матрица - это набор чисел, записанный таблицей. Матрицами могут быть с легкостью описаны любые модели в пространстве,
 * в том числе и модели движения.
 * 
 * Практически полезными операциями над матрицами являются:
 *    - сложение матрицы со скаляром
 *    - сложение нескольких матриц
 *    - умножение матрицы на скаляр
 *    - умножение двух матриц
 *    - умножение матрицы на вектор
 * 
 * Полезными также являются некоторые частные случаи матриц:
 *    - единичная матрица - матрица, главная диагональ которой заполнена единицами, а остальные элементы нулями.
 *      Единичная матрица третьего порядка описывает три ортогональных единичных вектора. Для согласованности с векторами четвертого
 *      порядка на практике используется единичная матрица 4-го порядка.
 *    - матрица масштабирования - это матрица 4x4, в которой элемент в четвертой строке и четвертом столбце заполнен единицей,
 *      другие элементы главной диагонали заполнены масштабирующими коэффициентами и оставшиеся элементы заполнены нулями.
 *      При умножении любого согласованного вектора на масштабирующую матрицу, его размеры изменяются пропорционально масштабирующим коэффициентам.
 *    - матрица сдвига - это матрица 4х4, в которой 4-ый столбец представляет собой вектор сдвига, главная диагональ заполнена единицами,
 *      а оставшиеся элементы заполнены нулями. При умножении любого согласованного вектора на матрицу сдвига, этот вектор параллельно перемещается
 *      в пространстве на указанное в векторе сдвига расстояние по трем осям.
 *    - матрица вращения - это матрица 4x4, при умножении на которую вектор поворачивается в пространстве на некоторый произвольный угол вокруг некоторой оси.
 *      Общий вид матрицы вращения имеет довольно тяжелую форму. Обычно вращение производится вокруг конкретной оси.
 * 
 * Для получения сложного движения матрица масштабирования, матрица сдвига и матрица вращения могут быть скомбинированы через умножение. Обычно порядок
 * выбирается такой: сначала производится масштабирование, затем вращение и наконец смещение.
 * 
 * Для работы с матрицами и векторами используется GLM библиотека (OpenGL Mathematics). GLM представлена заголовочными файлами и код функций встраивается в
 * приложение. В этом примере показаны приемы работы с GLM для создания простой анимации.
 */

#include "application.h"
#include "shader.h"
#include "SOIL.h"

#include <iostream>
//-------------------
// Для работы с GLM достаточно подключить следующие заголовки
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//-------------------
BEGIN_APP_DECLARATION(Transformations)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);

    Transformations() 
    : base(),
    pos_x(0.0f),
    pos_y(0.0f),
    scale (0.5) {}
protected:
    Shader* m_Shaders;
    GLuint VBO, VAO, EBO;
    GLuint texture_box;
    GLfloat pos_x, pos_y;
    GLfloat scale;
END_APP_DECLARATION()

DEFINE_APP(Transformations, "Geometric Transformations")

#define SHADER_PATH_PREFIX    "../shaders"
#define TEXTURE_PATH_PREFIX   "../textures"

void Transformations::gInit(const char* title) {
    base::gInit(title);
    /*
     * В этом примере мы накладываем одну текстуру и используем фрагментный шейдер из 5-го примера
     */
    if (!(m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader07.vs.glsl", 
                                 SHADER_PATH_PREFIX"/3.3.shader05.fs.glsl"))) {
        throw std::logic_error("something wrong with shaders");
    }
    /*
     * Модель движения подготавливается в основной программе и передается в шейдер вершин с помощью
     * uniform. Когда на сцене много движущихся объектов, то для каждого из них вычисляется матрица
     * трансформации каждый раз на новом этапе перерисовки сцены.
     * 
     * Преимущество от такого подхода в том, что вершины объектов хранятся в неизменном
     * виде и никуда не перемещаются. Изменяется только матрица трансформации, которую передавать в шейдер
     * оказывается намного быстрее. При этом имитация движения происходит за счет аккумуляции предыдущих 
     * вычислений матрицы трансформации.
     */
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
    
    // Мы устанавливаем интервал смены буферов побольше, чтобы анимация проигрывалась более плавно
    glfwSwapInterval(8);
} // gInit

void Transformations::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_Shaders->use();
    
    // подготовка текстуры
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_box);
    // передача текстуры во фрагментный шейдер
    m_Shaders->setInt("ourTexture",0);
    
    // Подготовка матрицы трансформации
    /*
     * ВАЖНО:
     * Следует помнить, что трансформации применяются в обратном порядке, нежели вы объявляете их в коде.
     * Например, ниже мы сдвигаем наш ящик, затем поворачиваем его и масштабируем , но на самом деле ящик масштабируется, затем
     * повернется, а потом переместится. Мы используем функцию glfwGetTime для запроса текущего времени,
     * чтобы создать простейшую анимацию вращения. Используйте стрелки, чтобы ящик двигался в плоскости.
     * Используйте кнопки 9 и 0 для масштабирования.
     */
    glm::mat4 transform;
    /*
     * Перемещение в плоскости. Мы используем translate чтобы сгенерировать матрицу сдвига.
     * Вектор, который передается во втором аргументе, является радиус-вектором точки, в котороую происходит сдвиг.
     */
    transform = glm::translate(transform, glm::vec3(pos_x, pos_y, 0.0f));    
    /*
     * Вектор, на который мы умножаем матрицу поворота, является обозначает ось, вокруг которой производится
     * вращение. В данном случае вращение вокруг оси z.
     */
    transform = glm::rotate(transform, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    /*
     * Масштабирование. Вектор, который передается вторым аргументом заполняется масштабирующими коэффициентами.
     */
    transform = glm::scale(transform, glm::vec3(scale, scale, scale));
    // Передача матрицы в шейдер вершин
    GLint transformLoc = glGetUniformLocation(m_Shaders->ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    /*
     * Примечание:
     * Обратите внимание, что мы используем шаблон glm::value_ptr для передачи указателя на матрицу.
     * Это делается для того, чтобы преобразовать значения в матрице к виду, который требует OpenGL.
     */
    // Рисование
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    base::gRender();
} // gRender

void Transformations::gFinalize() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    if (m_Shaders)
        delete m_Shaders;
    base::gFinalize();
} // gFinalize

void Transformations::onKey(int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            pos_y += 0.1f;
            if (pos_y > 1.0f)
                pos_y = -1.0f;
        }
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            pos_y -= 0.1f;
            if (pos_y < -1.0f)
                pos_y = 1.0f;
        }
        else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            pos_x -= 0.1f;
            if (pos_x < -1.0f)
                pos_x = 1.0f;
        }
        else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            pos_x += 0.1f;
            if (pos_x > 1.0f)
                pos_x = -1.0f;
        }
        
        if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
            scale += 0.1f;
            if (scale > 1.0f) 
                scale = 1.0f;
        } else if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
            scale -= 0.1f;
            if (scale < 0.0f) 
                scale = 0.0f;
        }

} // onKey
