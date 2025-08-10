#pragma once
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

