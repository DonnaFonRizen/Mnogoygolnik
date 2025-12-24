#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>

class Shader {
public:
    // Конструктор: загружает вершинный и фрагментный шейдеры из файлов
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Активировать шейдерную программу
    void Use() const;

    // Установка uniform-переменных (перегрузки для разных типов)
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, float v0, float v1) const;          // vec2
    void SetUniform(const std::string& name, float v0, float v1, float v2) const; // vec3
    void SetUniform(const std::string& name, float v0, float v1, float v2, float v3) const; // vec4
    void SetUniform(const std::string& name, const float* value, int count) const; // массив float
    // Можно добавить другие: матрицы, int-массивы и т.д.

private:
    GLuint programID_;

    // Вспомогательная функция для чтения файла
    static std::string ReadFile(const std::string& filepath);

    // Компиляция отдельного шейдера
    static GLuint CompileShader(GLenum type, const std::string& source);

    // Линковка программы
    static GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
};