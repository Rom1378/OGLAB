#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>



namespace ShaderManager
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

    void loadConfigs(const std::string& configPath)
    {
        try
        {
            // Read the entire file into a string
            std::ifstream file(configPath);
            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open shader config file: " + configPath);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string jsonContent = buffer.str();


            // Parse JSON with error checking
            auto jsonConfig = nlohmann::json::parse(jsonContent, nullptr, true, true);

            // Clear existing configs
            shaderConfigs.clear();

            // Load new configs
            for (const auto& [shaderName, shaderData] : jsonConfig.items())
            {
                ShaderConfig config;

                // Validate required fields
                if (!shaderData.contains("vertex") || !shaderData.contains("fragment"))
                {
                    throw std::runtime_error("Shader '" + shaderName + "' missing vertex or fragment path");
                }

                config.vertexPath = shaderData["vertex"].get<std::string>();
                config.fragmentPath = shaderData["fragment"].get<std::string>();

                // Optional defines
                if (shaderData.contains("defines") && shaderData["defines"].is_object())
                {
                    for (const auto& [defineName, defineValue] : shaderData["defines"].items())
                    {
                        config.defines[defineName] = defineValue.get<std::string>();
                    }
                }


                // Verify shader files exist
                std::ifstream vertFile(config.vertexPath);
                std::ifstream fragFile(config.fragmentPath);
                if (!vertFile.good())
                {
                    throw std::runtime_error("Vertex shader not found: " + config.vertexPath);
                }
                if (!fragFile.good())
                {
                    throw std::runtime_error("Fragment shader not found: " + config.fragmentPath);
                }

                shaderConfigs[shaderName] = config;
                std::cout << "Loaded config for shader: " << shaderName << std::endl;
            }
        }
        catch (const nlohmann::json::parse_error& e)
        {
            throw std::runtime_error("JSON parse error in " + configPath + ": " + e.what());
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Error loading shader configs: " + std::string(e.what()));
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