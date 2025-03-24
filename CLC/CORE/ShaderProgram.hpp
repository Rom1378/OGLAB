#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    // Prevent copying
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    // Load shaders from files
    void loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Add preprocessor define before shader compilation
    void addDefine(const std::string& name, const std::string& value);
    
    // Shader program usage
    void use() const;
    GLuint getProgram() const { return m_program; }

    // Uniform setters
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, float x, float y, float z);
	void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat4(const std::string& name, const float* value);
    //textures
	void setTexture(const std::string& name, int texture, int slot);
	void setBool(const std::string& name, bool value);

private:
    GLuint m_program;
    std::unordered_map<std::string, std::string> m_defines;

    // Helper functions
    GLuint compileShader(const std::string& source, GLenum type);
    std::string loadShaderSource(const std::string& path);
    std::string processShaderSource(const std::string& source);
    void checkCompileErrors(GLuint shader, const std::string& type);
};
