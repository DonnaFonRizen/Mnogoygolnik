
#define GLEW_DLL
#define GLFW_DLL

#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Model.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float fov = 45.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
float sensitivity = 0.1f;

float delta_time = 0.0f;
float last_frame = 0.0f;

// Переменные для управления роботом
float slide = 0.0f;           
float manipAngle = 0.0f;      
float gripperAngle = 0.0f;    

// Точки вращения 
glm::vec3 pivotManipToBeam(2.7765f, 3.9442f, 2.0021f);
glm::vec3 pivotGripperToManip(2.7872f, 4.1649f, 6.1184f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float moveSpeed = delta_time * 2.0f;   // скорость перемещения
    float rotSpeed = delta_time * 50.0f;  // скорость вращения (градусов/сек)

    // Управление линейным перемещением несущей балки (клавиши 1 и 2)
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        slide += moveSpeed;
        if (slide > 0.0f) slide = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        slide -= moveSpeed;
        if (slide < -3.0f) slide = -3.0f;
    }

    // Управление поворотом манипулятора (клавиши 3 и 4)
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        manipAngle += rotSpeed;
        if (manipAngle > 45.0f) manipAngle = 45.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        manipAngle -= rotSpeed;
        if (manipAngle < -30.0f) manipAngle = -30.0f;
    }

    // Управление поворотом кисти (клавиши 5 и 6)
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        gripperAngle += rotSpeed;
        if (gripperAngle > 40.0f) gripperAngle = 40.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        gripperAngle -= rotSpeed;
        if (gripperAngle < -40.0f) gripperAngle = -40.0f;
    }
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "4PU 900VH", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewExperimental = GL_TRUE;

    GLenum ret = glewInit();
    if (ret != GLEW_OK) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(ret));
        return 1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    Shader shader("vertex.glsl", "fragment.glsl");
    if (shader.GetProgram() == 0) {
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    Model ourModel("4PU.obj");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window)) {
       
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Use();

        float currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        processInput(window);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);

        unsigned int viewLoc = glGetUniformLocation(shader.GetProgram(), "view");
        unsigned int projLoc = glGetUniformLocation(shader.GetProgram(), "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

       
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "material.ambient"), 0.3f, 0.1f, 0.5f);
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "material.diffuse"), 0.7f, 0.2f, 0.9f);
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "material.specular"), 1.0f, 0.5f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.GetProgram(), "material.shininess"), 64.0f);

        // Свет
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shader.GetProgram(), "light.specular"), 1.0f, 1.0f, 1.0f);

        // Массивы матриц для каждой детали  0-станина, 1-балка, 2-манипулятор, 3-кисть
        glm::mat4 modelMatrices[4];

        // 0: станина 
        modelMatrices[0] = glm::mat4(1.0f);

        // 1: несущая балка 
        modelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(slide, 0.0f, 0.0f));

        // 2: манипулятор 
        {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(slide, 0.0f, 0.0f));
            m = glm::translate(m, pivotManipToBeam);
            m = glm::rotate(m, glm::radians(manipAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            m = glm::translate(m, -pivotManipToBeam);
            modelMatrices[2] = m;
        }

        // 3: кисть 
        {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(slide, 0.0f, 0.0f));
            m = glm::translate(m, pivotManipToBeam);
            m = glm::rotate(m, glm::radians(manipAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            m = glm::translate(m, -pivotManipToBeam);
            m = glm::translate(m, pivotGripperToManip);
            m = glm::rotate(m, glm::radians(gripperAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            m = glm::translate(m, -pivotGripperToManip);
            modelMatrices[3] = m;
        }

        ourModel.Draw(shader, modelMatrices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}