// подключаем библиотеки
#define GLEW_DLL
#define GLFW_DLL

#include <iostream>
#include <cmath>               // для sin, cosf
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"            // ваша библиотека для загрузки шейдеров

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    // Указание версии OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Mainwindow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    GLenum ret = glewInit();
    if (ret != GLEW_OK) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(ret));
        return 1;
    }

    
    // Загрузка шейдеров из файлов с помощью библиотеки Shader
    
    Shader shader("vertex.glsl", "fragment.glsl");

    
    // Подготовка геометрии фигуры
 
    float points[] = {
        -0.4f,  0.0f,  0.0f,   // вершина 0
        -0.2f,  0.4f,  0.0f,   // вершина 1
         0.2f,  0.4f,  0.0f,   // вершина 2
         0.4f,  0.0f,  0.0f,   // вершина 3
         0.2f, -0.4f,  0.0f,   // вершина 4
        -0.2f, -0.4f,  0.0f    // вершина 5
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5
    };

    GLuint vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    

    
    // Основной цикл рендеринга
    
    while (!glfwWindowShouldClose(window)) {
        // Очистка экрана
        glClearColor(1.0f, 1.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Активация шейдерной программы
        shader.Use();

        // Изменение цвета от времени
        float timeValue = static_cast<float>(glfwGetTime());
        float greenValue = (std::sin(timeValue) / 2.3f) + 0.5f;
        float redValue = (std::cos(timeValue) / 2.1f) + 0.5f;

        // Пересылка uniform-переменной в шейдер
        shader.SetUniform("ourColor", redValue, greenValue, 0.3f, 1.0f);

        // Рендеринг
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    

    glfwTerminate();
    return 0;
}