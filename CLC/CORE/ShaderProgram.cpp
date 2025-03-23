#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

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
    try {
        // Load shader sources
        std::string vertexSource = loadShaderSource(vertexPath);
        std::string fragmentSource = loadShaderSource(fragmentPath);

        // Process sources
        vertexSource = processShaderSource(vertexSource);
        fragmentSource = processShaderSource(fragmentSource);

        // Compile shaders
        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

        // Link program
        if (m_program) {
            glDeleteProgram(m_program);
        }
        m_program = glCreateProgram();

        glAttachShader(m_program, vertexShader);
        glAttachShader(m_program, fragmentShader);
        glLinkProgram(m_program);

        // Check linking errors
        GLint success;
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[1024];
            glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            throw std::runtime_error("Program linking failed");
        }

        // Clean up shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading shaders: " << e.what() << std::endl;
        throw;
    }
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

    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::string shaderType = (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed");
    }

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
    std::string source = buffer.str();

    return source;
}

std::string ShaderProgram::processShaderSource(const std::string& source)
{
    std::stringstream processed;

    // Version directive MUST come first
    processed << "#version 460 core\n";

    // Add defines after version but before the rest of the code
    for (const auto& [name, value] : m_defines) {
        processed << "#define " << name << " " << value << "\n";
    }

    // Add the shader code, but skip its version directive if it has one
    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("#version") == std::string::npos) {
            processed << line << "\n";
        }
    }

    return processed.str();
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

void ShaderProgram::setTexture(const std::string& name, int texture, int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(m_program, name.c_str()), slot);
}

void ShaderProgram::setBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}