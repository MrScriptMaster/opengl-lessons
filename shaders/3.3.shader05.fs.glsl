#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

/*
    Шейдер получает текстурные координаты от вершинного шейдера. Чтобы указать фрагментному шейдеру
    где хранится текстура, мы используем встроенный тип sampler2D и uniform.
    Через uniform мы указываем на расположение текстуры в памяти. Само изменение
    значения в uniform произодится неявно во время привязки текстуры в основной
    программе.
*/

void main()
{
/*
    Для сэмплирования необходимо применять встроенную в GLSL функцию texture. Первым аргументом
    передается текстура, а вторым текстурные координаты.
*/
//     FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
    FragColor = texture(ourTexture, TexCoord);
}
