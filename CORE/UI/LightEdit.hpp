#pragma once


#include <imgui.h>
#include "CORE/Systems/Renderer/Renderer.hpp"
#include "CORE/ComponentManager.hpp"
#include "CORE/Components/Light.hpp"
#include "CORE/Scene.hpp"
#include <string>

namespace UI {

    void renderLightControlMenu(Scene& scene) {
        ImGui::Begin("Light Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Scene Lighting Controls");
        ImGui::Separator();

        Renderer::RendererSettings& rdSetting = Renderer::getRendererSettings();

        ImGui::Checkbox("Enable Lighting", &rdSetting.enableLighting);
        ImGui::Checkbox("Enable Blinn-Phong", &rdSetting.enableBlinn);

        ImGui::Separator();
        ImGui::Text("Individual Lights");

        auto lightView = scene.getComponentView<LightComponent>();

        int lightIndex = 0;
        for (auto [entity, light] : lightView) {
            ImGui::PushID(lightIndex);

            TransformComponent* transform = scene.getComponent<TransformComponent>(entity);

            // Get entity name for display
            std::string lightName = "Light " + std::to_string(lightIndex);
            if (light->type == LightComponent::Type::POINT) lightName += " (Point)";
            else if (light->type == LightComponent::Type::DIR) lightName += " (Directional)";
            else if (light->type == LightComponent::Type::SPOT) lightName += " (Spot)";

            if (ImGui::CollapsingHeader(lightName.c_str())) {
                // Position (for point and spot lights)
                if (transform && light->type != LightComponent::Type::DIR) {
                    ImGui::DragFloat3("Position", &transform->pos.x, 0.1f);
                }

                // Direction (for directional and spot lights)
                if (light->type == LightComponent::Type::DIR) {
                    auto* dirData = std::get_if<DirLightData>(&light->data);
                    if (dirData) {
                        ImGui::DragFloat3("Direction", &transform->rot.x, 0.01f);
                    }
                }
                else if (light->type == LightComponent::Type::SPOT) {
                    auto* spotData = std::get_if<SpotLightData>(&light->data);
                    if (spotData) {
                        ImGui::DragFloat3("Direction", &transform->rot.x, 0.01f);
                        float cutOffDeg = glm::degrees(glm::acos(spotData->cutOff));
                        float outerCutOffDeg = glm::degrees(glm::acos(spotData->outerCutOff));
                        if (ImGui::DragFloat("Inner Cutoff (deg)", &cutOffDeg, 0.5f, 0.0f, 90.0f)) {
                            spotData->cutOff = glm::cos(glm::radians(cutOffDeg));
                        }
                        if (ImGui::DragFloat("Outer Cutoff (deg)", &outerCutOffDeg, 0.5f, 0.0f, 90.0f)) {
                            spotData->outerCutOff = glm::cos(glm::radians(outerCutOffDeg));
                        }
                    }
                }

                // Colors
                ImGui::ColorEdit3("Ambient", &light->ambient.x);
                ImGui::ColorEdit3("Diffuse", &light->diffuse.x);
                ImGui::ColorEdit3("Specular", &light->specular.x);

                // Attenuation (for point and spot lights)
                if (light->type == LightComponent::Type::POINT) {
                    auto* pointData = std::get_if<PointLightData>(&light->data);
                    if (pointData) {
                        ImGui::DragFloat("Constant", &pointData->constant, 0.01f, 0.0f, 10.0f);
                        ImGui::DragFloat("Linear", &pointData->linear, 0.001f, 0.0f, 1.0f);
                        ImGui::DragFloat("Quadratic", &pointData->quadratic, 0.0001f, 0.0f, 1.0f);
                    }
                }
                else if (light->type == LightComponent::Type::SPOT) {
                    auto* spotData = std::get_if<SpotLightData>(&light->data);
                    if (spotData) {
                        ImGui::DragFloat("Constant", &spotData->constant, 0.01f, 0.0f, 10.0f);
                        ImGui::DragFloat("Linear", &spotData->linear, 0.001f, 0.0f, 1.0f);
                        ImGui::DragFloat("Quadratic", &spotData->quadratic, 0.0001f, 0.0f, 1.0f);
                    }
                }
            }

            ImGui::PopID();
            lightIndex++;
        }

        ImGui::Separator();

        // Light statistics
        int numDir = 0, numPoint = 0, numSpot = 0;
        for (auto [entity, light] : lightView) {
            if (light->type == LightComponent::Type::DIR) numDir++;
            else if (light->type == LightComponent::Type::POINT) numPoint++;
            else if (light->type == LightComponent::Type::SPOT) numSpot++;
        }

        ImGui::Text("Total Lights: %d", lightIndex);
        ImGui::Text("  Directional: %d", numDir);
        ImGui::Text("  Point: %d", numPoint);
        ImGui::Text("  Spot: %d", numSpot);

        ImGui::End();

    }

}