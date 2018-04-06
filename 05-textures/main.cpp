/*
 * Текстура - это двухмерное изображение, которое может быть наложено на некоторую поверхность в трехмерной сцене
 * для создания большего реализма графики.
 * 
 * Если бы текстур не существовало, то нам бы требовалось раскрашивать каждый фрагмент по отдельности, что было бы не
 * возможным, так как нужны колосальные размеры памяти для хранения цвета каждого фрагмента. Текстуры же сами несут
 * на себе все необходимые цвета. Кроме того, реалистичная текстура может сократить число вершин для модели, так
 * как некоторые детали могут быть нарисованы на самой текстуре.
 * 
 * Текстуры могут накладываться на различные геометрические формы при этом OpenGL по сути берет у текстуры цвет для
 * фрагмента. Процесс отбора цветов у текстуры называется "сэмплированием" (от англ. sampling - отбор). Не все цвета
 * текстуры могут быть отобраны, например, потому что текстура имеет размер больше, чем поверхность.
 * 
 * Чтобы указать правило отбора цветов, мы должны определить координаты текстуры, которые должны образовать замкнутую фигуру.
 * 
 * Текстурные координаты изменяются в промежутке от 0.0 до 1.0. Когда координаты выходят за этот диапазон, то по умолчанию
 * OpenGL повторяет текстуру. Этим приемом пользуются, когда объект имеет однородную структуру, например стеклянное окно или
 * кирпичная стена. Тем не менее, можно задать иное поведение: ничего не рисовать, отзеркалить структуру или растянуть
 * по краю.
 * 
 * Текстура может быть наложена на трехмерный объект. Она в этом случае называется трехмерной, но остается плоской. В этом
 * случае реалистичность накладывания заключается в загибании краев текстуры по некоторому правилу.
 * 
 * Отбор цветов у текстуры это еще не все, что происходит при наложении. Другой частью наложения является фильтрация цветов.
 * Фильтрация цветов обычно делается, когда текстура с низким разрешением накладывается на очень большую поверхность. В этом
 * случае фрагментов поверхности оказывается намного больше, чем цветов на текстуре и OpenGL необходимо знать, как ему отбирать
 * при этом цвета. В самом простом случае имеются "фильтрация по ближайшему соседу" - выбирается цвет текстуры, ближайший к
 * текущей координате поверхности - и (би)линейная фильтрация, когда цвет интерполируется из ближайших к центру текселя (точки текструры)
 * цветов, который ближе всего к фрагменту поверхности. При (би)линейной фильтрации наложенная текстура кажется немного
 * размытой в местах контура, но если поверхность далеко от наблюдателя, эта размытость сильно не выделяется.
 * 
 * В реальных проектах текстуры для одного объекта сцены не хранят в одном экземляре, а делают мипмапы (mipmaps), когда в одном изображении
 * одна текстура хранится с разным размером и разрешением на определенных позициях. Когда сцена наблюдается с далекого расстояния нет нужды накладывать текстуру
 * с большим разрешением, потому что наблюдатель все равно не разглядит детали. Тогда для наложения в мипмапе берется текстура
 * небольшого размера и меньшим разрешением, по мере приближения наблюдателя к объекту, должна накладываться текстура все большим и большим
 * разрешением. Мипмапы заготавливаются вручную, а разработчик затем подсказывает OpenGL по каким правилам накладывать текстуры из
 * мипмапа.
 */

#include "application.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION        // эта директива необходима в нашем случае: реализация функций будет внедрена в этот файл
#include "stb_image.h"

#include <iostream>

BEGIN_APP_DECLARATION(Textures)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
protected:
    Shader* m_Shaders;
    unsigned int VBO, VAO, EBO;
    unsigned int texture;
END_APP_DECLARATION()

DEFINE_APP(Textures, "Textures")

#define SHADER_PATH_PREFIX    "../shaders"
#define TEXTURE_PATH_PREFIX   "../textures"

void Textures::gInit(const char* title) {
    base::gInit(title);
    
    if (!(m_Shaders = new Shader(SHADER_PATH_PREFIX"/3.3.shader05.vs.glsl", 
                                 SHADER_PATH_PREFIX"/3.3.shader05.fs.glsl"))) {
        throw std::logic_error("something wrong with shaders");
    }
    float vertices[] = {
        // positions        // colors          // texture coords
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 2.0f, // top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 2.0f  // top left
        };
    /*
     * Мы построим прямоугольник по двум треугольикам и наложим на него текстуру
     */
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    /*
     * Теперь у нас три вида атрибутов
     */
    // координаты вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // цвета вершин
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // координаты для наложения текструры
    /*
     * Обратите внимание, что координаты текстуры имеют смещение в 6 полей на каждой строке
     */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    /*
     * Настройка текстуры
     */
    // генерация буфера текстуры
    glGenTextures(1, &texture);
    // привязка текстуры, чтобы все дальнейшие операции были направлены на нее
    glBindTexture(GL_TEXTURE_2D, texture);
    // настройка поведения, когда текстура не соизмерима с поверхностью (здесь она будет повторяться)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // настройка фильтрации (линейная)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /*
     * Загрузка текструры. Для загрузки мы используем библиотеку stb
     */
    int width, height, nrChannels;
    unsigned char *data = stbi_load(TEXTURE_PATH_PREFIX"/wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // загрузка текструры в память
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // теперь мы можем освободить память, так как текстура загружена в OpenGL контекст
    stbi_image_free(data);
    
} // gInit

void Textures::gRender(bool auto_redraw) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // привязка текструры
    glBindTexture(GL_TEXTURE_2D, texture);
    m_Shaders->use();
    // рисуем прямоугольник
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

