#include "CORE/UI/Physics.hpp"
#include "imgui.h"

#include "CORE/Systems/Physics/Physics.hpp"

namespace UI {
	void imgui_physics_menu() {
		ImGui::Begin("Physics");

		static bool enableDebugVisualization = true;

		if (ImGui::Button(enableDebugVisualization ? "Disable Physics Debug Visualization" : "Enable Physics Debug Visualization")) {
			enableDebugVisualization = !enableDebugVisualization;
			PhysicsSystem::enable_debug_visualization(enableDebugVisualization);
		}
		PhysicsSystem::imgui_debug_menu();

		ImGui::End();

	}
}