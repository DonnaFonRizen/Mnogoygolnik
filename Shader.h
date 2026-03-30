#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Use() const;
    GLuint GetProgram() const { return programID_; }
    bool load(const std::string& vertexPath, const std::string& fragmentPath);

    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, float v0, float v1) const;
    void SetUniform(const std::string& name, float v0, float v1, float v2) const;
    void SetUniform(const std::string& name, float v0, float v1, float v2, float v3) const;
    void SetUniform(const std::string& name, const float* value, int count) const;

    GLuint programID_;
private:
    static std::string ReadFile(const std::string& filepath);
    static GLuint CompileShader(GLenum type, const std::string& source);
    static GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
};