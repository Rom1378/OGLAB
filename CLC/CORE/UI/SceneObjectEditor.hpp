#pragma once





void renderImGuiObjectEditor(GameObject* selectedObject) {
	if (!selectedObject) return;

	// Create a window for the object editor
	if (ImGui::Begin("Object Editor")) {
		ImGui::Text("Selected: %s", selectedObject->getName());

		// Position editor
		glm::vec3 position = selectedObject->getPosition();
		if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
			selectedObject->setPosition(position, true); // true to update physics
		}

		// Option 1: Edit using Euler angles (more intuitive)
		glm::vec3 rotation = selectedObject->getRotation();
		if (ImGui::DragFloat3("Rotation (Euler)", glm::value_ptr(rotation), 1.0f)) {
			selectedObject->setRotation(rotation, true); // true to update physics
		}

		// Option 2: Edit using quaternion (more precise, but less intuitive)
		glm::quat rotQuat = selectedObject->getRotationQuaternion();
		float quatValues[4] = { rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w };
		if (ImGui::DragFloat4("Rotation (Quaternion)", quatValues, 0.01f)) {
			// Normalize the quaternion to prevent distortion
			glm::quat newQuat(quatValues[3], quatValues[0], quatValues[1], quatValues[2]);
			newQuat = glm::normalize(newQuat);
			selectedObject->setRotationQuaternion(newQuat, true);
		}

		// Add a button to normalize the quaternion explicitly
		if (ImGui::Button("Normalize Quaternion")) {
			glm::quat normalized = glm::normalize(selectedObject->getRotationQuaternion());
			selectedObject->setRotationQuaternion(normalized, true);
		}

		// Scale editor
		glm::vec3 scale = selectedObject->getScale();
		if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
			selectedObject->setScale(scale);
		}
	}
	ImGui::End();
}