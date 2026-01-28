#pragma once

#include <glm/glm.hpp>
#include <variant>

#include "CORE/Lights/LightManager.hpp"
#include "CORE/Transform.hpp"

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

struct LightComponent {
    enum class Type { POINT, DIR, SPOT };

    LightComponent() {
    }

    Type type{};
    glm::vec3 ambient{1.,1.,1.};
    glm::vec3 diffuse{1.,1.,1.};
    glm::vec3 specular{1.,1.,1.};

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


/*#pragma once
#include "../TransformableComponent.hpp"
#include <memory>

enum class LightType { POINT, DIRECTIONAL, SPOT };

class Light : public TransformableComponent, public std::enable_shared_from_this<Light> {
public:

    Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity);
    //~Light();

    
    void init() override;

    void update(float dt) override;

    LightType getType() const { return type; }
    void setType(LightType type) { this->type = type; }

    glm::vec3 getColor() const { return color; }
    void setColor(glm::vec3 color) { this->color = color; }

    float getIntensity() const { return intensity; }
    void setIntensity(float intensity) { this->intensity = intensity; }

    void onImGuiRender() override;

    const char* getName() override { return "Light"; }


private:
    LightType type;
    glm::vec3 color;
    float intensity;

};
*/
