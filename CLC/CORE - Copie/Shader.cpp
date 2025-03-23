#include "Shader.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace MyNamespace
{
    namespace
    {
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_shaders;

        struct ShaderConfig
        {
            std::string vertexPath;
            std::string fragmentPath;
            std::unordered_map<std::string, std::string> defines;
        };

        std::unordered_map<std::string, ShaderConfig> shaderConfigs;
    }

    void loadConfigs(const std::string &configPath)
    {
        std::ifstream file(configPath);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open shader config file: " + configPath);
        }

        nlohmann::json jsonConfig;
        file >> jsonConfig;

        for (const auto& [shaderName, shaderData] : jsonConfig.items())
        {
            ShaderConfig config;
            config.vertexPath = shaderData["vertex"].get<std::string>();
            config.fragmentPath = shaderData["fragment"].get<std::string>();

            // Load defines if they exist
            if (shaderData.contains("defines"))
            {
                for (const auto& [defineName, defineValue] : shaderData["defines"].items())
                {
                    config.defines[defineName] = defineValue.get<std::string>();
                }
            }

            shaderConfigs[shaderName] = config;
        }
    }

    // Get or load a shader
    std::shared_ptr<ShaderProgram> getShader(const std::string &name)
    {
        if (m_shaders.find(name) == m_shaders.end())
        {
            loadShader(name);
        }
        return m_shaders[name];
    }

    // Load a specific shader
    void loadShader(const std::string &name)
    {
        if (shaderConfigs.find(name) == shaderConfigs.end())
        {
            throw std::runtime_error("No config found for shader: " + name);
        }

        const auto &config = shaderConfigs[name];
        auto shader = std::make_shared<ShaderProgram>();

        // Add any preprocessor defines
        for (const auto &[define, value] : config.defines)
        {
            shader->addDefine(define, value);
        }

        shader->loadFromFiles(config.vertexPath, config.fragmentPath);
        m_shaders[name] = shader;
    }

    // Reload all shaders (useful for hot-reloading)
    void reloadAll()
    {
        for (const auto &[name, _] : m_shaders)
        {
            loadShader(name);
        }
    }

    // Clear all loaded shaders
    void cleanup()
    {
        m_shaders.clear();
        shaderConfigs.clear();
    }
} // namespace MyNamespace