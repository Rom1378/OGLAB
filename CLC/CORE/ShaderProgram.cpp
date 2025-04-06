#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <glm/gtc/type_ptr.hpp>

// Static variable initialization
bool GLDebug::debugCallbackInitialized = false;

// Debug callback implementation
void GLAPIENTRY GLDebug::debugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam) {

    // Get source string
    const char* sourceStr;
    switch (source) {
    case GL_DEBUG_SOURCE_API: sourceStr = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER: sourceStr = "Other"; break;
    default: sourceStr = "Unknown"; break;
    }

    // Get type string
    const char* typeStr;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: typeStr = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "Undefined Behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY: typeStr = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "Performance"; break;
    case GL_DEBUG_TYPE_OTHER: typeStr = "Other"; break;
    default: typeStr = "Unknown"; break;
    }

    // Get severity string
    const char* severityStr;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: severityStr = "HIGH"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW: severityStr = "LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "NOTIFICATION"; break;
    default: severityStr = "UNKNOWN"; break;
    }

    // Only print errors or high severity messages by default
    if (type == GL_DEBUG_TYPE_ERROR || severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cerr << "[OpenGL Debug] " << severityStr << " " << typeStr
            << " from " << sourceStr << ": " << message << std::endl;
    }
}

void GLDebug::init() {
    if (debugCallbackInitialized) return;

    // If OpenGL 4.3+ is available, use the debug callback
    if (GLAD_GL_VERSION_4_3) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback, nullptr);
        // Configure the types of messages to show
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
            GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR,
            GL_DONT_CARE, 0, nullptr, GL_TRUE);
        debugCallbackInitialized = true;
        std::cout << "OpenGL debug output enabled" << std::endl;
    }
    else {
        std::cout << "OpenGL debug output not available (requires OpenGL 4.3+)" << std::endl;
    }
}

void GLDebug::checkError(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                   error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                  error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:              error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                 error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:                error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                  error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                                error = "UNKNOWN_ERROR"; break;
        }
        std::cerr << "[OpenGL Error] (" << error << " | " << errorCode << "): "
            << file << ":" << line << std::endl;
    }
}

void GLDebug::clearErrors() {
    while (glGetError() != GL_NO_ERROR);
}

ShaderProgram::ShaderProgram() : m_program(0) {
    // Initialize debug capabilities
    static std::once_flag debugInitFlag;
    std::call_once(debugInitFlag, []() {
        GLDebug::init();
        });

    GL_CLEAR_ERROR();
    m_program = glCreateProgram();
    GL_CHECK_ERROR();
}

ShaderProgram::~ShaderProgram() {
    if (m_program) {
        GL_CLEAR_ERROR();
        glDeleteProgram(m_program);
        GL_CHECK_ERROR();
    }
}

GLint ShaderProgram::getUniformLocation(const std::string& name) const {
    GL_CLEAR_ERROR();
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    GL_CHECK_ERROR();

    if (loc == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found or optimized out" << std::endl;
    }
    return loc;
}

GLuint ShaderProgram::compileShaderInternal(const std::string& source, GLenum type, const char* typeName) {
    GL_CLEAR_ERROR();
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GL_CHECK_ERROR();

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER::" << typeName << "::COMPILATION_FAILED\n" << infoLog << "\n";
        std::cerr << "Shader source:\n" << source << "\n";
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed");
    }
    return shader;
}

GLuint ShaderProgram::compileShader(const std::string& source, GLenum type) {
    const char* typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" :
        ((type == GL_FRAGMENT_SHADER) ? "FRAGMENT" :
            ((type == GL_GEOMETRY_SHADER) ? "GEOMETRY" : "UNKNOWN"));
    GLuint shader = compileShaderInternal(source, type, typeName);
    return shader;
}

std::string ShaderProgram::loadShaderSource(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

std::string ShaderProgram::processShaderSource(const std::string& source) {
    std::stringstream processed;
    processed << "#version 460 core\n";

    for (const auto& [name, value] : m_defines) {
        processed << "#define " << name << " " << value << "\n";
    }

    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("#version") == std::string::npos) {
            processed << line << "\n";
        }
    }
    return processed.str();
}
        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

        if (m_program) {
            GL_CLEAR_ERROR();
            glDeleteProgram(m_program);
            GL_CHECK_ERROR();
        }

        GL_CLEAR_ERROR();
        m_program = glCreateProgram();
        glAttachShader(m_program, vertexShader);
        glAttachShader(m_program, fragmentShader);
        glLinkProgram(m_program);
        GL_CHECK_ERROR();

        GLint success;
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[1024];
            glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
            throw std::runtime_error("Program linking failed");
        }

        GL_CLEAR_ERROR();
        glValidateProgram(m_program);
        GL_CHECK_ERROR();

        glGetProgramiv(m_program, GL_VALIDATE_STATUS, &success);
        if (!success) {
            GLchar infoLog[1024];
            glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM::VALIDATION_FAILED\n" << infoLog << "\n";
        }

        GL_CLEAR_ERROR();
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        GL_CHECK_ERROR();
    }
    catch (const std::exception& e) {
        std::cerr << "Shader loading error: " << e.what() << std::endl;
        throw;
    }
}

void ShaderProgram::use() const {
    GL_CLEAR_ERROR();
    glUseProgram(m_program);
    GL_CHECK_ERROR();
}

void ShaderProgram::addDefine(const std::string& name, const std::string& value) {
    m_defines[name] = value;
}

// Uniform setters
void ShaderProgram::setInt(const std::string& name, int value) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniform1i(loc, value);
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setFloat(const std::string& name, float value) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniform1f(loc, value);
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniform3f(loc, x, y, z);
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniform3fv(loc, 1, glm::value_ptr(value));
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniform4f(loc, x, y, z, w);
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& value) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setMat4(const std::string& name, const float* value) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniformMatrix4fv(loc, 1, GL_FALSE, value);
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setTexture(const std::string& name, int texture, int slot) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(loc, slot);
        GL_CHECK_ERROR();
    }
}

void ShaderProgram::setBool(const std::string& name, bool value) {
    if (GLint loc = getUniformLocation(name); loc != -1) {
        GL_CLEAR_ERROR();
        glUniform1i(loc, value);
        GL_CHECK_ERROR();
    }
}