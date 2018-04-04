
/* 
 * Пример взят из упражнения: https://learnopengl.com/Getting-started/Hello-Triangle
 * 
 * Конвейер
 * -----------------------
 * В этом примере мы нарисуем несколько треугольников, чтобы понять как устроен конвейер OpenGL.
 * 
 * Прежде чем что либо нарисовать на экране, драйвер графической карты должен проделать много однотипной работы в конвейере.
 * В конвейер входит 6 этапов:
 * 1. Исполнение шейдера вершин - на этом этапе на вход конвейера поступают координаты вершин. Небольшая программа, которая называется шейдером вершин, переводит их условные координаты в физические координаты на экране или, другими словами, в пиксели.
 * 2. Построитель примитивов - преобразованные вершины соединяются линиями. Обычно сложные фигуры строятся из простых, таких как треугольники. Эти простые фигуры называются "примитивами". Говоря просто, на этом этапе вершины соединяются линиями по некоторому правилу.
 * 3. Геометрический шейдер - на этом этапе из уже построенных примитивов строятся новые, в зависимости от программы, написанной разработчиком.
 * 4. Растеризация - на этом этапе фигуры превращаются во фрагменты или массив из пикселей. У каждого пикселя во фрагменте есть свой набор атрибутов. Также на этом этапе выпоняется вырезка фрагментов (clipping), т.е. отбрасываются те фрагменты, которые при данном наблюдении не видны.
 * 5. Исполнение шейдера фрагментов - на этом этапе вычисляются цвета фрагментов и могут быть применены некоторые эффекты, такие как затенение или отражение.
 * 6. Тесты и смешение - на этом этапе каждый фрагмент проходит тесты: тест глубины, чтобы понять какой фрагмент ближе к наблюдателю, а какой дальше; "стенсил тест", когда рисование происходит по трафарету; тест на прозрачность. На заключительном этапе происходит смешение цветов и реализуется окончательное изображение. В результате смешения цвет фрагмента может измениться.
 * 
 * Разработчик может изменять или конфигурировать шейдеры на 1, 3 и 5 этапах. Остальные этапы выполняются автоматически, но некоторое поведение вы можете изменять с помощью функций OpenGL.
 * 
 * В современном OpenGL разработчик должен минимум предоставить конвейеру вершинный шейдер (1 этап) определить шейдер фрагментов (5 этап), т.к. эти части конвейера не предоставляют стандартной реализации.
 */
 
#include "application.h"

BEGIN_APP_DECLARATION(Triangles)
    virtual void gInit(const char* title = NULL);
    virtual void gRender(bool auto_redraw = true);
    virtual void gFinalize();
    void onKey(int key, int scancode, int action, int mods);
protected:
    unsigned int VBO, VAO, EBO;
    int shaderProgram;
    int switcher;
    int mode;
END_APP_DECLARATION()

DEFINE_APP(Triangles, "Triangles")

//-----------------------------------------------------
/*
 * Чтобы нарисовать что-нибудь, необходимо указать координаты контура фигуры, либо
 * узлы этой фигуры. OpenGL работает в трехмерной декартовой системе координат, поэтому
 * для каждой вершины нужно задать координаты x, y и z. При плоском рисовании следует делать
 * координату z нулевой, т.е. у сцены нет глубины.
 * 
 * Чтобы задать систему координат используется область просмотра (viewport). В OpenGL
 * положительным направлением для оси x считается слева-направо, для оси y - снизу вверх, а для оси
 * z - от заднего фона экрана к наблюдателю. Координаты следует передавать в нормализованном виде, т.е.
 * их допустимые значения лежат в промежутке от 0.0 до (+/-)1.0. Все что выходит за эти пределы считается
 * невидимым и обрезается в случае, если система координат не перемещается.
 * 
 * Началом отсчета по умолчанию является середина viewport по всем осям.
 *
 * Участок памяти на графической карте, который используется для хранения вершин, называется буферизованным
 * вершинным объектом (vertex buffer objects (VBO)). Шейдеры исполняются на процессоре графической карты,
 * поэтому они получают доступ к VBO черезвычайно быстро.
 */

/*
 * Шейдеры
 * 
 * Шейдеры составляются на языке GLSL похожий на C. Шейдеры компилируются на ходу, поэтому код шейдера
 * поставляется программе в виде строки.
 * 
 * Ниже показано два шейдера: вершинный и шейдер фрагментов. Все шейдеры должны
 * начинаться с номера версии OpenGL, для которой они поставляются.
 */

/*
 * Следующий вершинный шейдер демонстрирует общую структуру шейдера вообще.
 * Первая строка шейдера объявляет версию OpenGL для которой он написан. В этом примере
 * это версия 3.3.0 (330).
 * 
 * У шейдера выделяются входы и выходы. В этом шейдере всего один вход, который объявляется
 * через оператор in. В данном случае мы принимаем в виде трехмерного вектора вершину, для
 * ссылки на которую будет использоваться имя aPos. С помощью оператора layout (location = 0) мы указываем
 * место ее хранения.
 * 
 * После объявления входа начинается тело шейдера, которое оформляется в функции main. Переменная
 * gl_Position является предопределенным вектором размерности 4 (vec4), в котором первые три значения
 * указывают координату вершины, а четвертое - делитель перспективы. В данном случае перспективы нет,
 * поэтому мы должны передавать 1.0. gl_Position является выходом шейдера. В этом примере шейдер ничего
 * не делает с входной вершиной, а просто переписывает ее как есть в выходную переменную.
 */
const char* vertexShaderS = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
    "}\0";

/* 
 * Шейдер фрагментов также прост как и шейдер вершин. В этом примере мы объявляем выходную переменную
 * FragColor. В OpenGL цвет получается по схеме RGB, т.е. через сочетание красного, зеленого и синего
 * полутонов, где единица означает максимальный оттенок (красный, зеленый или синий). 
 * Четвертный параметр выходного текста отвечает за прозрачность, где 1 - это полностью не прозрачный.
 * 
 * В этом шейдере мы присваиваем каждой вершине одинаковый цвет (в данном случае оранжевый).
 */
const char* fragmentShaderS = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void Triangles::gInit(const char* title) {
    base::gInit(title);
    glfwSwapInterval(1);
    
    switcher = 0;
    mode = 0;
    
    // Сборка и компиляция шейдеров
    int success = 0;
    char infoLog[512];
    // Шейдер вершин
    int vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShaderS, NULL);
    glCompileShader(vShader);
    // узнаем результат компиляции шейдера
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        std::cout << "Shader: " << infoLog << std::endl;
        throw std::logic_error("can't compile vertex shader");
    }
    // шейдер фрагментов
    int fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShaderS, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
         std::cout << "Shader: " << infoLog << std::endl;
        throw std::logic_error("can't compile fragment shader");
    }
    // Линковка шейдеров
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    // узнаем результат линковки
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Linking shaders: " << infoLog << std::endl;
        throw std::logic_error("failed shaders linking");
    }
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    
    // определяем вершины для треугольников. Треугольники смежны по гипотенузе, поэтому после заливки
    // мы увидим прямоугольник. Середина этого прямоугольника совпадает с центром начала координат
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // верхний правый угол
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    // индексы для индексного рисвания
    // при индексном рисовании мы подсказываем OpenGL по индексам в каком порядке рисовать вершины
    // Мы используем индексы, чтобы нарисовать прямоугольник из двух треугольников по одному набору вершин
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    // генерируем массив (Vertex Array Object), который будет хранить атрибуты.
    // VAO должен ассоциироваться с VBO.
    glGenVertexArrays(1, &VAO);
    // генерируем буфер для вершин
    glGenBuffers(1, &VBO);
    // генерируем буфер для элементов
    glGenBuffers(1, &EBO);
    
    // привязывем буфер вершин
    glBindVertexArray(VAO);
    // привязываем буфер для вершин к контексту. С каждым VBO буфером должен быть связан один VAO буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // передаем в буфер координаты вершин и указываем стратегию их перерисовывания
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // буфер элементов привязывается 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /*
     * Следующей функцией мы подсказываем OpenGL как нужно читать поток из входных
     * вершин:
     *  - первый аргумент - где размещать вершины. Этот 0 соотносится со строкой layout (location = 0)
     *    в шейдере вершин
     *  - второй аргумент - сколько элементов массива нужно записать в один вектор. В шейдере используется
     *    vec3 поэтому мы записываем 3.
     *  - третий аргумент - какой тип данных используется в векторе. Здесь это GL_FLOAT.
     *  - четвертый аргумент - нужно ли нормировать значения вершин. Мы устанавливаем GL_FALSE, потому
     *    что вершины передаются в уже нормированном виде.
     *  - пятый аргумент - это шаг (stride). После того как одна вершина обработана в массиве, мы говорим на сколько 
     *    нужно элементов массива шагнуть, чтобы начать интерпретировать новую вершину. В данном случае
     *    3*sizeof(float) байт.
     *  - шестой аргумент - это смещение, чтобы показать с какой позиции начинаются данные внутри буфера.
     *    В этом примере нам смещение не требуется.
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    /*
     * Включаем атрибуты вершин
     */
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
}    
    
void Triangles::gRender(bool auto_redraw) {
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    switch (mode) {
        case 1:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 0:
        default:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    // draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    switch(switcher)
    {
        case 1:
            // эта функция завставляет рисовать из буфера вершин
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        case 2:
            glDrawArrays(GL_TRIANGLES, 1, 3);
            break;
        case 3:
            glDrawArrays(GL_TRIANGLES, 2, 3);
            break;
        case 0:
        default:
            // эта функция заставляет OpenGL рисовать из буфера индексов
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
    }
    // glBindVertexArray(0); // no need to unbind it every time 
 
    base::gRender(auto_redraw);
}

void Triangles::onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        switcher = 0;
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS) 
        switcher = 1;
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS) 
        switcher = 2;
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS) 
        switcher = 3;
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS) 
        mode = 1;
    else if (key == GLFW_KEY_9 && action == GLFW_PRESS) 
        mode = 0;
}

void Triangles::gFinalize() {
    // освобождаем буферы вершин
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    base::gFinalize();
}