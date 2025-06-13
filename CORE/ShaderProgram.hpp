#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <mutex>

// Debug helper macros
#ifdef _DEBUG
#define GL_CHECK_ERROR() GLDebug::checkError(__FILE__, __LINE__)
#define GL_CLEAR_ERROR() GLDebug::clearErrors()
#define GL_LOG_CALL(x) GLDebug::clearErrors(); x; GLDebug::checkError(__FILE__, __LINE__)
#else
#define GL_CHECK_ERROR()
#define GL_CLEAR_ERROR()
#define GL_LOG_CALL(x) x
#endif

// OpenGL debug utilities
class GLDebug {
public:
    static void init();
    static void checkError(const char* file, int line);
    static void clearErrors();

private:
    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
    static bool debugCallbackInitialized;
};

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    // Prevent copying
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    // Loading and configuration
    void loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void addDefine(const std::string& name, const std::string& value);
    void use() const;
    GLuint getProgram() const { return m_program; }

    // Uniform setters
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat4(const std::string& name, const glm::mat4& value);
    void setMat4(const std::string& name, const float* value);
    void setTexture(const std::string& name, int texture, int slot);
    void setBool(const std::string& name, bool value);

private:
    GLuint m_program;
    std::unordered_map<std::string, std::string> m_defines;

    // Helper functions
    GLuint compileShader(const std::string& source, GLenum type);
    std::string loadShaderSource(const std::string& path);
    std::string processShaderSource(const std::string& source);

    // Debug helpers
    GLint getUniformLocation(const std::string& name) const;
    GLuint compileShaderInternal(const std::string& source, GLenum type, const char* typeName);
};