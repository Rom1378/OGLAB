#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "ShaderProgram.hpp"

namespace ShaderManager
{
    std::shared_ptr<ShaderProgram> getShader(const std::string &name);
    void loadShader(const std::string &name);
    void loadConfigs(const std::string &configPath);
    void reloadAll();
    void cleanup();
}