#include "CubeRenderer.hpp"
#include "Mesh/CubeMesh.hpp"

#include "LightManager.hpp" 
void CubeRenderer::init() {

	// Generate and bind VAO, VBO, and EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Bind and fill VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshData::Cube::vertices.size(), MeshData::Cube::vertices.data(), GL_STATIC_DRAW);

	// Bind and fill EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * MeshData::Cube::indices.size(), MeshData::Cube::indices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Set default shader
	setShader("standard");
	m_shader->use();


}

void CubeRenderer::draw(const std::shared_ptr<Camera> cam) {
	if (!m_shader) return;

	m_shader->use();

	// Set matrix uniforms
	m_shader->setMat4("model", glm::value_ptr(this->getGameObject()->getModelMatrix()));
	m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
	m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));

	// Handle textures
	m_shader->setBool("useTexture", !m_textures.empty());
	if (!m_textures.empty())
		bindTextures();

	// Handle lighting
	std::vector<std::shared_ptr<Light>>
		relevantLights = LightManager::getRelevantLights(cam, 128);
	bool useLighting = !relevantLights.empty();

	m_shader->setBool("useLighting", useLighting);
	m_shader->setInt("numLights", relevantLights.size());

	for (size_t i = 0; i < relevantLights.size(); i++) {
		std::string base = "lights[" + std::to_string(i) + "].";

		// Set light type
		int lightType = 0; // Default to Point Light
		switch (relevantLights[i]->getType()) {
		case LightType::POINT: lightType = 0; break;
		case LightType::DIRECTIONAL: lightType = 1; break;
		case LightType::SPOT: lightType = 2; break;
		}
		m_shader->setInt(base + "type", lightType);

		m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
		m_shader->setVec3(base + "direction", relevantLights[i]->getDirection());
		m_shader->setVec3(base + "color", relevantLights[i]->getColor());
		m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());
		}

	// Set object color
	m_shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);

	// Draw cube
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Unbind textures if used
	if (!m_textures.empty())
		unBindTextures();
}