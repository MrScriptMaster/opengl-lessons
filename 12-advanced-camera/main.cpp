/*
 * В предыдущем примере мы изучили как менять камеру на сцене. В этом примере мы рассмотрим как управлять
 * камерой более продвинуто.
 * 
 * Напомним, что для расчета Look At матрицы нам необходимо иметь вектор позиции, вектор направления и вектор, лежащий
 * на верхней оси камеры.
 * 
 * Общий подход для вычисления позиции камеры такой
 * 
 *      glm::vec3   camPosition = glm::vec3(...);
 *      glm::vec3   camFront    = glm::vec3(...);
 *      glm::vec3   camUp       = glm::vec3(...);
 * 
 *      glm::mat4   view = glm::lookAt(camPosition, camPosition + camFront, camUp);
 * 
 * Обратите внимание, что вектор направления это в общем случае сумма вектора позиции камеры и нормали к плоскости,
 * образуемой верхней и праввой осями камеры.
 * 
 * Трехмерная сцена в каждый конкретный момент наблюдается из некоторой камеры. Допустим, что камера это глаза
 * наблюдателя. Как реализуется иллюзия свободы перемещения наблюдателя в пространстве сцены?
 * Перемещение зрителя в разные стороны без изменения направления взгляда и вертикальной ориентации (другими словами хождение без
 * поворотов головой) реализуется достаточно просто. При движении в разные стороны достаточно изменять вектор camPosition по следующим формулам:
 * 
 *      camPosition += ratioSpeed * camFront;                                       // движение вперед по направлению камеры
 *      camPosition -= ratioSpeed * camFront;                                       // движение назад (пятиться)
 *      camPosition -= glm::normalize(glm::cross(camFront, camUp)) * ratioSpeed;    // идти боком влево
 *      camPosition += glm::normalize(glm::cross(camFront, camUp)) * ratioSpeed;    // идти боком вправо
 * 
 * где
 *      ratioSpeed - коэффициент скорости. Чем он больше, тем быстрее перемещается камера в некотором направлении.
 *      В этом примере скорость по всем направлениям получается одинаковой, однако, обычно на практике каждое направление
 *      использует свой коэффициент, так как, для примера, некоторые объекты могут двигаться вперед быстрее, чем назад.
 * 
 * Функция glm::normalize вычисляет нормаль по двум векторам. Обратите внимание, что мы выполняем простые операции с векторами. Для
 * движения вправо или вслево нам необходимо предварительно вычислить нормаль к плоскости, образуемой осью направления камеры и верхней осью камеры. В зависимости
 * от направления движения мы меняем знак нормали. Вычислений нормали при движении вперед и назад не нужно, потому что camFront по определению
 * является нормалью.
 * 
 * Скорость движения камеры
 * ----------------------------
 * К сожалению нельзя просто так изменять ratioSpeed, потому что на разных типах оборудования скорость отрисовки кадра может быть столь медленной
 * (например, потому что кроме отрисовки работает другая тяжелая программа, занявшая ресурсы компьютера), что пользователь будет замечать подвисания - 
 * когда его перемещение откликается на экране с видимой задержкой или рывками. Вы могли это также замечать в предыдущих примерах.
 * Это происходит, потому что наше рисование привязано к процессорному времени, которое в многозадачной системе никогда не распределяется равномерно.
 * Чтобы избавиться от этого эффекта при движении камеры, коэффициент должен синхронизироваться с текущей частотой смены кадров.
 * 
 * Все, что происходит на экране в графической программе должно синхронизироваться с часами программы. В GLFW запрос текущего времени программы производится
 * функцией glfwGetTime(). На основе этой функции ведется расчет разницы между кадрами.
 * Общий подход синхронизации скорости камеры должен быть таким
 * 
 *      float deltaTime = 0.0f;         // разница времени между кадрами
 *      float lastFrameTime = 0.0f;     // момент времени рисования предыдущего кадра
 *      ....
 *      float currentFrameTime = glfwGetTime();          // запрос времени на момент рисования текущего кадра
 *      deltaTime = currentFrameTime - lastFrameTime;    // расчет разницы между кадрами
 *      lastFrameTime = currentFrameTime;                // обновляем момент рисования предыдущего кадра
 *      ....
 *      ratioSpeed = 2.5f * deltaTime;                   // синхронизация скорости движения камеры
 * 
 * Поправочный коэффициент может использоваться, чтобы имитировать бег или спокойный шаг.
 * 
 * Вращение камеры
 * ----------------------------
 * Мы рассмотрели как камера движется в разные стороны. Рассмотрим теперь вращение камеры. В трехмерном пространстве у камеры есть три угла,
 * на которые она способна поворачиваться: рыскание (поворот вокруг верхней оси), тангаж (поворот вокруг правой оси) и крен (поворот вокруг оси направления).
 * Повороты по отдельности реализуются довольно просто через простые формулы тригонометрии. Во всех случаях мы вносим корректировку в составляющие вектора
 * направления.
 * 
 *      Тангаж (pitch) и рыскание (yaw)
 *      direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
 *      direction.y = sin(glm::radians(pitch));
 *      direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
 * 
 *      Крен
 *      Кренение осуществляется через вектор camUp.
 * 
 * Управление углами с помощью мыши
 * ----------------------------------
 * В задачи управления мышью входит перерасчет линейных координат перемещения в значения для углов тангажа и рыскания (кренение мышью выполнить впринципе нельзя).
 * Алгоритм здесь такой
 * 
 *      1. Рассчитать разницу между позицией мыши в предыдущем кадре и текущей позицией по горизонтали и вертикали.
 *      Примечание: для вертикали нужно делать вычитание из последней позиции текущей, потому что ось начинается снизу, а для
 *      горизонтали нужно наоборот из текущей координаты вычитать последнюю.
 *      2. Обе разницы умножаются на поправочный коэффициент, который называется чувствительностью мыши.
 *      3. К углу рыскания прибавляется разница по горизонтали.
 *      4. К углу тангажа разница по вертикали. 
 *      5. Дальше поворот камеры вычисляется по обычным формулам.
 * 
 *      glm::vec3 front;
 *      front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
 *      front.y = sin(glm::radians(pitch));
 *      front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
 *      camFront = glm::normalize(front);      
 *      
 * 
 * Примечание:
 *      На углы рыскания и тангажа необходимо наложить ограничения от -89 градусов до +89 градусов.
 * 
 * Приближение и удаление с помощью камеры
 * ----------------------------------------
 * Приближение и удаление изменяется через угол перспективы. Уменьшая угол будет создаваться эффект удаления.
 * 
 * Ниже показаны все приемы управления камерой "в лоб". В следующем примере вы найдете тот же самый код реализованный с помощью
 * класса Camera (camera.h).
 */

#include "application.h"
#include "shader.h"
#include "SOIL.h"
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

/*
 * Объявление обработчика событий мыши - движение
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
/*
 * Объявление обработчика событий мыши - скроллинг
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//----------------------------------------------------------------------------
// Настройка камеры
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;            // угол тангажа
float lastX =  800.0f / 2.0;    // позиция курсора мыши по горизонтали
float lastY =  600.0f / 2.0;    // позиция курсора мыши по вертикали
float fov   =  45.0f;           // угол перспективы

// timing
float deltaTime = 0.0f;         // разница между отрисовкой кадров
float lastFrame = 0.0f;         // момент отрисовки последнего кадра
//----------------------------------------------------------------------------
void Cube::gInit(const char* title) {
    base::gInit(title);
    // Подключаем обработчики мыши
    glfwSetCursorPosCallback(m_pWindow, mouse_callback);
    glfwSetScrollCallback(m_pWindow, scroll_callback);
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
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    m_Shaders->setMat4("view", view);
    
    // настройка проекции
    projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
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
    float cameraSpeed = 2.5 * deltaTime;
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos += cameraSpeed * cameraFront;
    else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos -= cameraSpeed * cameraFront;
    else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
} // onKey

//---------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    /*
     * Это проверка нужна, чтобы избежать рывка мыши во время первого запуска
     */
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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
} // mouse_callback

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Управление углом перспективы
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
} // scroll_callback
