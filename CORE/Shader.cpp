#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "CORE/Debug.hpp"


namespace ShaderManager
{
    namespace
    {
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_shaders;

        struct ShaderConfig
        {
            std::string vertexPath;
            std::string fragmentPath;
            std::string geometryPath; // Optional
            std::unordered_map<std::string, std::string> defines;
        };

        std::unordered_map<std::string, ShaderConfig> shaderConfigs;
    }

    void loadConfigs(const std::string& configPath)
    {
        try
        {
            std::ifstream file(configPath);
            if (!file.is_open())
            {
                LOG_ERROR("Failed to open shader config file: " + configPath);
                throw std::runtime_error("Failed to open shader config file: " + configPath);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string jsonContent = buffer.str();

            auto jsonConfig = nlohmann::json::parse(jsonContent, nullptr, true, true);

            shaderConfigs.clear();

            for (const auto& [shaderName, shaderData] : jsonConfig.items())
            {
                ShaderConfig config;

                if (!shaderData.contains("vertex") || !shaderData.contains("fragment"))
                {
                    LOG_ERROR("Shader '" + shaderName + "' missing vertex or fragment path");
                    throw std::runtime_error("Shader '" + shaderName + "' missing vertex or fragment path");
                }

                config.vertexPath = shaderData["vertex"].get<std::string>();
                config.fragmentPath = shaderData["fragment"].get<std::string>();

                // Optional geometry shader
                if (shaderData.contains("geometry"))
                {
                    config.geometryPath = shaderData["geometry"].get<std::string>();
                }

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
                    LOG_ERROR("Vertex shader not found: " + config.vertexPath);
                    throw std::runtime_error("Vertex shader not found: " + config.vertexPath);
                }
                if (!fragFile.good())
                {
                    LOG_ERROR("Fragment shader not found: " + config.fragmentPath);
                    throw std::runtime_error("Fragment shader not found: " + config.fragmentPath);
                }

                if (!config.geometryPath.empty())
                {
                    std::ifstream geomFile(config.geometryPath);
                    if (!geomFile.good())
                    {
                        LOG_ERROR("Geometry shader not found: " + config.geometryPath);
                        throw std::runtime_error("Geometry shader not found: " + config.geometryPath);
                    }
                }

                shaderConfigs[shaderName] = config;
                LOG("Loaded config for shader: ", shaderName);
            }
        }
        catch (const nlohmann::json::parse_error& e)
        {
            LOG_ERROR("JSON parse error in " + configPath + ": " + e.what());
            throw std::runtime_error("JSON parse error in " + configPath + ": " + e.what());
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Error loading shader configs: " + std::string(e.what()));
            throw std::runtime_error("Error loading shader configs: " + std::string(e.what()));
        }
    }

    std::shared_ptr<ShaderProgram> getShader(const std::string& name)
    {
        if (m_shaders.find(name) == m_shaders.end())
        {
            loadShader(name);
        }
        return m_shaders[name];
    }

    void loadShader(const std::string& name)
    {
        if (shaderConfigs.find(name) == shaderConfigs.end())
        {
            throw std::runtime_error("No config found for shader: " + name);
        }

        const auto& config = shaderConfigs[name];
        auto shader = std::make_shared<ShaderProgram>();

        for (const auto& [define, value] : config.defines)
        {
            shader->addDefine(define, value);
        }

        // Load with or without geometry shader
        if (!config.geometryPath.empty())
        {
            shader->loadFromFiles(config.vertexPath, config.fragmentPath, config.geometryPath);
        }
        else
        {
            shader->loadFromFiles(config.vertexPath, config.fragmentPath);
        }

        m_shaders[name] = shader;
    }

    void reloadAll()
    {
        for (const auto& [name, _] : m_shaders)
        {
            loadShader(name);
        }
    }

    void cleanup()
    {
        m_shaders.clear();
        shaderConfigs.clear();
    }
}
