#include "../Lights/LightManager.hpp"
#include "CubeRenderer.hpp"
#include "../Mesh/CubeMesh.hpp"

void CubeRenderer::init() {
	m_isShadowCaster= true;
	m_isShadowReceiver = true;

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
	renderWithMaterials(cam);
	}


void CubeRenderer::renderRawGeometry(const glm::mat4& lightSpaceMatrix) {
	// Use simple depth shader (no materials/textures/lighting)
	auto depthShader = ShaderManager::getShader("simpleDepthShader");
	depthShader->use();

	// Only need model and light space matrix
	depthShader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));
	depthShader->setMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));

	// Draw bare geometry
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void CubeRenderer::renderWithMaterials(const std::shared_ptr<Camera>& cam) {
	if (!m_shader) return;

	m_shader->use();

	// Standard matrix uniforms
	m_shader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));
	m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
	m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));
	m_shader->setMat4("lightSpaceMatrix", LightManager::getShadowMapper()->getLightSpaceMatrix());

	// Material properties
	m_shader->setBool("useTexture", !m_textures.empty());
	m_shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);
	m_shader->setVec3("viewPos", cam->getPosition());

	// Bind textures if available
	if (!m_textures.empty()) {
		//bindTextures();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textures[0]->id);
		m_shader->setInt("texture_diffuse1", 0);
	}

	// Lighting (same as your existing draw() code)
	std::vector<std::shared_ptr<Light>> relevantLights = LightManager::getRelevantLights(cam, 128);
	m_shader->setBool("useLighting", !relevantLights.empty());
	m_shader->setInt("numLights", relevantLights.size());


	for (size_t i = 0; i < relevantLights.size(); i++) {
		std::string base = "lights[" + std::to_string(i) + "].";
		m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
		m_shader->setVec3(base + "color", relevantLights[i]->getColor());
		m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());
	}
	/*

	for (size_t i = 0; i < relevantLights.size(); i++) {
		std::string base = "lights[" + std::to_string(i) + "].";

		// Set light type
		int lightType = 0; // Default to Point Light
		switch (relevantLights[i]->getType()) {
		case LightType::POINT: lightType = 0; break;
		case LightType::DIRECTIONAL: lightType = 1; break;
		case LightType::SPOT: lightType = 2; break;
		}
		//m_shader->setInt(base + "type", lightType);

		m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
		//m_shader->setVec3(base + "direction", relevantLights[i]->getDirection());
		//m_shader->setVec3(base + "color", relevantLights[i]->getColor());
		//m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());

	}*/

	// Bind shadow map (now handled by LightManager)
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, LightManager::getShadowMapper()->getDepthMapTexture());
	m_shader->setInt("shadowMap", 1);
	m_shader->setVec3("lightPos", relevantLights[0]->getPosition());
	//LightManager::bindShadowMap(m_shader);

	// Draw
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	// Reset texture binding
	glActiveTexture(GL_TEXTURE0);
}


CubeRenderer::~CubeRenderer() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}