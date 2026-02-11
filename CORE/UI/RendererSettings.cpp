#include "RendererSettings.hpp"
#include "CORE/Systems/Renderer/PostProcessing.hpp"
#include <imgui.h>

namespace UI {
	void postProcessingMenu() {
		ImGui::Begin("Post Processing", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		bool enableGamma = PostProcessing::isGammaCorrectionEnabled();

		if (ImGui::Checkbox("Enable Gamma Correction", &enableGamma)) {
			PostProcessing::enableGammaCorrection(enableGamma);
		}

		if (enableGamma) {
			float gammaValue = PostProcessing::getGamma();
			if (ImGui::DragFloat("Gamma Value", &gammaValue, 0.1f, 0.1f, 5.0f)) {
				PostProcessing::setGamma(gammaValue);
			}
		}

		ImGui::End();


	}

}