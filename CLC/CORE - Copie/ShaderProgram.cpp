#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

ShaderProgram::ShaderProgram() : m_program(0)
{
    m_program = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    if (m_program)
        glDeleteProgram(m_program);
}

void ShaderProgram::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Load shader sources
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);

    // Process sources (add defines)
    vertexSource = processShaderSource(vertexSource);
    fragmentSource = processShaderSource(fragmentSource);

    // Compile shaders
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    // Link program
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);
    checkCompileErrors(m_program, "PROGRAM");

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::addDefine(const std::string& name, const std::string& value)
{
    m_defines[name] = value;
}

void ShaderProgram::use() const
{
    glUseProgram(m_program);
}

GLuint ShaderProgram::compileShader(const std::string& source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

std::string ShaderProgram::loadShaderSource(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string ShaderProgram::processShaderSource(const std::string& source)
{
    std::stringstream processed;
    
    // Add version directive if not present
    if (source.find("#version") == std::string::npos)
    {
        processed << "#version 330 core\n";
    }

    // Add defines
    for (const auto& [name, value] : m_defines)
    {
        processed << "#define " << name << " " << value << "\n";
    }

    processed << source;
    return processed.str();
}

void ShaderProgram::checkCompileErrors(GLuint shader, const std::string& type)
{
    int success;
    char infoLog[1024];
    
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error("Shader compilation error (" + type + "): " + infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error("Shader program linking error: " + std::string(infoLog));
        }
    }
}

// Uniform setters implementation
void ShaderProgram::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(m_program, name.c_str()), x, y, z);
}

void ShaderProgram::setMat4(const std::string& name, const float* value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, value);
} 