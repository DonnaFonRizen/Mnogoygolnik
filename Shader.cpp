#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {

    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);

   
        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);


    programID_ = LinkProgram(vertexShader, fragmentShader);


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(programID_);
}

void Shader::Use() const {
    glUseProgram(programID_);
}


// Реализация чтения файла

std::string Shader::ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Компиляция шейдера
GLuint Shader::CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Проверка на ошибки
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "ERROR: Shader compilation failed\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


// Линковка программы

GLuint Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "ERROR: Program linking failed\n" << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    return program;
}


// Установка uniform-переменных

void Shader::SetUniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programID_, name.c_str()), value);
}

void Shader::SetUniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID_, name.c_str()), value);
}

void Shader::SetUniform(const std::string& name, float v0, float v1) const {
    glUniform2f(glGetUniformLocation(programID_, name.c_str()), v0, v1);
}

void Shader::SetUniform(const std::string& name, float v0, float v1, float v2) const {
    glUniform3f(glGetUniformLocation(programID_, name.c_str()), v0, v1, v2);
}

void Shader::SetUniform(const std::string& name, float v0, float v1, float v2, float v3) const {
    glUniform4f(glGetUniformLocation(programID_, name.c_str()), v0, v1, v2, v3);
}

void Shader::SetUniform(const std::string& name, const float* value, int count) const {
    // Пример для массивов: установка как vec-типа, в зависимости от count
    switch (count) {
    case 1: glUniform1fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    case 2: glUniform2fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    case 3: glUniform3fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    case 4: glUniform4fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    default:
        std::cerr << "Unsupported count for uniform array: " << count << std::endl;
    }
}