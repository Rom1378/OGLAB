#pragma once

#include <glm/glm.hpp>
#include <variant>

#include "CORE/Lights/LightManager.hpp"
#include "CORE/Transform.hpp"
#include "CORE/Systems/Renderer/ShadowMap.hpp"

struct SpotLightData {
    float cutOff;
    float outerCutOff;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct PointLightData {
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct DirLightData {

};

struct ShadowCasterComponent {
	ShadowMapSystem::ShadowMapHandle shadowMapHandle{ ShadowMapSystem::INVALID_SHADOWMAP_HANDLE };
    float near_plane{ 1.0f };
    float far_plane{ 1017.5f };
    uint32_t shadowMapResolution{ 2048 };
};

struct LightComponent {
    enum class Type { POINT, DIR, SPOT };

  
    Type type{};
    glm::vec3 ambient{0.0f,0.0f,0.0f };
    glm::vec3 diffuse{ 0.0f,0.0f,0.0f };
    glm::vec3 specular{ 0.0f,0.0f,0.0f };

    Transform localTransform{};

    std::variant<PointLightData, DirLightData, SpotLightData> data;

    // Helper methods to access data safely
    PointLightData* getPointData() {
        return std::get_if<PointLightData>(&data);
    }

    DirLightData* getDirData() {
        return std::get_if<DirLightData>(&data);
    }

    SpotLightData* getSpotData() {
        return std::get_if<SpotLightData>(&data);
    }
};
