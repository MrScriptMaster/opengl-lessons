/*
 * Текстура - это двухмерное изображение, которое может быть наложено на некоторую поверхность в трехмерной сцене
 * для создания большего реализма графики. Текстура состоит из текселей (пикселей текстуры).
 * 
 * Если бы текстур не существовало, то нам бы требовалось раскрашивать каждый фрагмент по отдельности. 
 * Кроме того, реалистичная текстура может сократить число вершин для модели, так
 * как некоторые детали могут быть нарисованы на самой текстуре.
 * 
 * Текстуры могут накладываться на различные геометрические формы. OpenGL по сути берет у текстуры цвет для
 * фрагмента. Процесс отбора цветов у текстуры называется "сэмплированием" (от англ. sampling - отбор). Не все цвета
 * текстуры могут быть отобраны однозначно, например, потому что текстура имеет размер больше, чем поверхность (в этом случае
 * будет применена фильтрация. См.ниже).
 * 
 * Чтобы указать правило отбора цветов, мы должны сообщить каждой вершине поверхности, какой части текстуры она принадлежит.
 * 
 * Текстурные координаты изменяются в промежутке от 0.0 до 1.0. Когда координаты выходят за этот диапазон, то по умолчанию
 * OpenGL повторяет текстуру. Этим приемом пользуются, когда объект имеет однородную структуру, например земля или
 * кирпичная стена. Тем не менее, можно задать иное поведение: ничего не рисовать, отзеркалить структуру или растянуть
 * по краю.
 * 
 * Текстура может быть наложена на трехмерный объект. Она в этом случае называется трехмерной, но остается плоской. В этом
 * случае реалистичность накладывания заключается в загибании краев текстуры по некоторому правилу.
 * 
 * Отбор цветов у текстуры это еще не все, что происходит при наложении. Другой частью наложения является фильтрация цветов.
 * Фильтрация цветов связана с тем, что тексельные координаты имеют тип с плавающей точкой и не зависят от разрешения текстуры. В этом случае OpenGL нужно некоторое правило, 
 * по которому у текстуры отбирается цвет, когда несколько текстурных координат нацеливаются на один и тот же тексель.
 * Фильтрация цветов обычно делается, когда текстура с низким разрешением накладывается на очень большую поверхность. В этом
 * случае фрагментов поверхности оказывается намного больше, чем цветов на текстуре и OpenGL необходимо знать, как ему отбирать
 * при этом цвета. В самом простом случае имеются "фильтрация по ближайшему соседу" - выбирается тексель, ближайший к
 * текущей координате поверхности - и (би)линейная фильтрация, когда цвет интерполируется из текселей, ближайших к центру фрагмента.
 * При (би)линейной фильтрации наложенная текстура кажется немного размытой в местах контура, но если поверхность далеко от наблюдателя, эта размытость сильно не выделяется.
 * 
 * В реальных проектах текстуры для одного объекта сцены не хранят в одном экземляре, а делают мипмапы (mipmaps), когда в одном буфере
 * одна текстура хранится с разным разрешением (обычно каждая последующая текстура вдвое меньше, чем предыдущая). Когда сцена наблюдается с далекого расстояния нет нужды накладывать текстуру
 * с большим разрешением, потому что наблюдатель все равно не разглядит детали. Накладывание текстуры с большим разрешением на сравнительно небольшую поверхность
 * мало того, что расходует много памяти, но и порождает видимые глазу графические артефакты, которые явно вредят визуализации.
 * Когда наблюдатель приближется к объекту, на него должна накладываться текстура с большим разрешением; когда отдаляется - с меньшим.
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
    /*
     * Для того, чтобы привязать текстуру к треугольнику мы должны сообщить каждой вершине треугольника, какой части 
     * текстуры принадлежит эта вершина. Каждая вершина, соответственно должна иметь текстурные координаты, ассоциированные с частью текстуры.
     * 
     * В этом примере текстурные координаты хранятся вместе с атрибутами-координатами и атрибутами цветами.
     * Мы накладываем текстуру по всему прямоугольнику.
     */
    float vertices[] = {
        // positions        // colors          // texture coords
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // top left
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
    /*
     * Оси наложения текстуры обозначаются через s, t и r (аналоги x, y и z). Когда какие-то
     * составляющие отсутствуют, то говорят 1D-, 2D- и 3D-текстура соответственно.
     * 
     * Функция glTexParameteri принимает следующие параметры:
     *  - тип текстуры (в данном сдучае 2D-текстура)
     *  - вторым аргументом мы определяем параметр для текструры, который хотим устновить.
     *    В данном случае первый вызов применяется для GL_TEXTURE_WRAP_S (выход текстурной координаты за границу по оси s).
     *  - третьим аргументом определяется значение для параметра. В данном случае мы говорим, чтобы текстура повторялась.
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // настройка фильтрации (линейная)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /*
     * Загрузка текструры. Для загрузки мы используем библиотеку SOIL (Simple OpenGL Image Library).
     * (https://github.com/nothings/stb).
     * 
     * При загрузке текстуры в память нет какого то одного решения. Все зависит от формата хранения текстуры.
     * В этом примере наша текстура хранится в формате jpeg. 
     */
    int width, height, nrChannels;
    /*
     * Первый аргумент - путь к изображению
     * Второй и третий аргументы - указатели для сохранения ширины и высоты изображения
     * Четвертый аргумент - указатель для сохранения числа каналов
     * Пятый аргумент - если нужны все каналы, то мы указываем 0
     */
    unsigned char *data = stbi_load(TEXTURE_PATH_PREFIX"/wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // Генерация текструры по загруженному ранее изображению
        /*
         * 1 - целевая текстура
         * 2 - уровень мипмапа. В данном случае 0 означает, что мы доверяем генерацию мипмапа OpenGL.
         * 3 - формат хранения текстуры в контексте OpenGL.
         * 4,5 - размеры результирующей текстуры. Просто передаем результат, полученный при загрузке.
         * 6 - устаревший аргумент - всегда 0.
         * 7 - формат изображения, которое мы передаем контексту. Не путать с 3 аргументом: мы
         *     просто сообщаем OpenGL в каком формате мы передаем функции байты, после загрузки изображения в память.
         * 8 - размер RGB значений (в данном случае в байтах).
         * 9 - указатель на буфер, в котором хранится ранее загруженное изображение.
         */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        /*
         * Для данной текстуры мы можем сгенерировать автоматический мипмап с помощью glGenerateMipmap. При использовании текстур из
         * мипмапа мы также можем применять фильтрацию после отбора мипмапа, но в этом примере это не имеет смысла, так как объект 
         * статичен.
         */
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
