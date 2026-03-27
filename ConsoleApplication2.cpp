// подключаем библиотеки
#define GLEW_DLL
#define GLFW_DLL

#include <iostream>
#include <cmath>               
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"            
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


// Глобальные переменные для управления камерой

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float yaw   = -90.0f;
float pitch =   0.0f;

float lastX = 1024.0f / 2.0f;
float lastY = 1024.0f / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


// Функция обратного вызова для обработки движения мыши
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевёрнуто
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

// Обработка клавиатуры

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraRight;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

// Вспомогательная функция для установки uniform-матрицы в шейдере
void setUniformMatrix4(GLuint program, const std::string& name, const glm::mat4& mat) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    } else {
        std::cerr << "Uniform " << name << " not found in shader!" << std::endl;
    }
}


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
    const unsigned int SCR_WIDTH = 1024;
    const unsigned int SCR_HEIGHT = 1024;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mainwindow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    GLenum ret = glewInit();
    if (ret != GLEW_OK) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(ret));
        return 1;
    }

    // Настройка ввода
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Загрузка шейдеров 
    Shader shader("vertex.glsl", "fragment.glsl");
    shader.Use(); 

    // Получаем идентификатор шейдерной программы 
    GLuint shaderProgram = shader.GetProgram(); 
    

    
    // Подготовка геометрии фигуры
    
    float points[] = {
        -0.4f,  0.0f,  0.0f,
        -0.2f,  0.4f,  0.0f,
         0.2f,  0.4f,  0.0f,
         0.4f,  0.0f,  0.0f,
         0.2f, -0.4f,  0.0f,
        -0.2f, -0.4f,  0.0f
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

    glEnable(GL_DEPTH_TEST);

    
    // Основной цикл
  
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(1.0f, 1.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use(); 

        // Матрицы
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 100.0f);

        // Передаём матрицы через прямой вызов OpenGL
        setUniformMatrix4(shaderProgram, "model", model);
        setUniformMatrix4(shaderProgram, "view", view);
        setUniformMatrix4(shaderProgram, "projection", projection);

        // Цвет
        float timeValue = static_cast<float>(glfwGetTime());
        float greenValue = (std::sin(timeValue) / 2.3f) + 0.5f;
        float redValue   = (std::cos(timeValue) / 2.1f) + 0.5f;
       
        GLint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
        if (colorLoc != -1) glUniform4f(colorLoc, redValue, greenValue, 0.3f, 1.0f);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}
