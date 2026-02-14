#include "RendererSettings.hpp"
#include "CORE/Systems/Renderer/PostProcessing.hpp"
#include <imgui.h>

namespace UI {
	void postProcessingMenu() {
		ImGui::Begin("Post Processing", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::SeparatorText("HDR & Tone Mapping");
		
		bool hdrEnabled = PostProcessing::isHDREnabled();
		if (ImGui::Checkbox("Enable HDR", &hdrEnabled)) {
			PostProcessing::enableHDR(hdrEnabled);
		}
		
		if (hdrEnabled) {
			ImGui::Indent();
			float exposure = PostProcessing::getExposure();
			if (ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f, "%.2f")) {
				PostProcessing::setExposure(exposure);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##exposure")) {
				PostProcessing::setExposure(1.0f);
			}
			
			ImGui::Text("Tone Mapping: ACES Filmic");
			ImGui::Unindent();
		}
		
		ImGui::Spacing();
		ImGui::SeparatorText("Gamma Correction");
		
		float gamma = PostProcessing::getGamma();
		if (ImGui::SliderFloat("Gamma", &gamma, 1.0f, 3.0f, "%.2f")) {
			PostProcessing::setGamma(gamma);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##gamma")) {
			PostProcessing::setGamma(2.2f);
		}
		
		ImGui::Spacing();
		ImGui::SeparatorText("Bloom");

		float bloomThreshold = PostProcessing::getBloomThreshold();
		if (ImGui::SliderFloat("Threshold", &bloomThreshold, 0.0f, 2.0f, "%.2f")) {
			PostProcessing::setBloomThreshold(bloomThreshold);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##threshold")) {
			PostProcessing::setBloomThreshold(0.5f);
		}

		float bloomIntensity = PostProcessing::getBloomIntensity();
		if (ImGui::SliderFloat("Intensity", &bloomIntensity, 0.0f, 3.0f, "%.2f")) {
			PostProcessing::setBloomIntensity(bloomIntensity);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##intensity")) {
			PostProcessing::setBloomIntensity(1.0f);
		}

		int bloomBlurPasses = PostProcessing::getBloomBlurPasses();
		if (ImGui::SliderInt("Blur Passes", &bloomBlurPasses, 1, 20)) {
			PostProcessing::setBloomBlurPasses(bloomBlurPasses);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##passes")) {
			PostProcessing::setBloomBlurPasses(10);
		}

		ImGui::Spacing();

		ImGui::TextDisabled("HDR renders in linear space (GL_RGBA16F)");
		ImGui::TextDisabled("Tone mapping converts HDR to LDR (0-1)");
		ImGui::TextDisabled("Gamma corrects for monitor display");
		
		ImGui::End();
	}
}