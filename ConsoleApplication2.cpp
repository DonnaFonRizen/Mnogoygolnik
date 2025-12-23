#define GLEW_DLL
#define GLFW_DLL

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>


int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(512, 512, "Mainwindow", NULL, NULL);

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


    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0, 1.0, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POLYGON);

        glVertex2f(-0.4, 0); glColor3f(0.2, 0.4, 1.0);
        glVertex2f(-0.2, 0.4);
        glVertex2f(0.2, 0.4);
        glVertex2f(0.4, 0);
        glVertex2f(0.2, -0.4);
        glVertex2f(-0.2, -0.4);

        glEnd();
        glfwSwapBuffers(window);

        glfwPollEvents();
        //glfwTerminate();
    }


    return 0;
}