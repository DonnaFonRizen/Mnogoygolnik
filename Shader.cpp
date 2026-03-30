#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm.hpp>

Shader::Shader() : programID_(0) {}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    load(vertexPath, fragmentPath);
}

Shader::~Shader() {
    if (programID_ != 0)
        glDeleteProgram(programID_);
}

void Shader::Use() const {
    glUseProgram(programID_);
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);
    if (vertexCode.empty() || fragmentCode.empty())
        return false;

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);
    if (vertexShader == 0 || fragmentShader == 0)
        return false;

    GLuint newProgram = LinkProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (newProgram == 0)
        return false;

    if (programID_ != 0)
        glDeleteProgram(programID_);
    programID_ = newProgram;
    return true;
}

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

GLuint Shader::CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

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
    switch (count) {
    case 1: glUniform1fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    case 2: glUniform2fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    case 3: glUniform3fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    case 4: glUniform4fv(glGetUniformLocation(programID_, name.c_str()), 1, value); break;
    default:
        std::cerr << "Unsupported count for uniform array: " << count << std::endl;
    }
}