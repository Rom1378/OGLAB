#include "SphereRenderer.hpp"
#include "../Lights/LightManager.hpp"



SphereRenderer::~SphereRenderer() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void SphereRenderer::renderRawGeometry(const glm::mat4& lightSpaceMatrix) {
	// Implement raw geometry rendering if needed
}
void SphereRenderer::renderWithMaterials(const std::shared_ptr<Camera>& cam) {
	if (!m_shader) return;

	m_shader->use();

	// Set matrix uniforms
	m_shader->setMat4("model", glm::value_ptr(this->getGameObject()->getModelMatrix()));
	m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
	m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));

	// Handle textures
	//m_shader->setBool("useTexture", !m_textures.empty());
	if (!m_textures.empty()) {
		bindTextures();
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


}

void SphereRenderer::init() {
	setShader("sphere");
	generateSphere(radius, sectorCount, stackCount);
	initBuffers();
}

void SphereRenderer::draw(const std::shared_ptr<Camera> cam) {
	if (!m_shader) return;

	m_shader->use();

	// Set uniforms
	m_shader->setMat4("model", glm::value_ptr(this->getGameObject()->getModelMatrix()));
	m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
	m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));

	std::vector<std::shared_ptr<Light>> relevantLights =
		LightManager::getRelevantLights(cam, 128);

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


	m_shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);

	// Draw sphere using indices
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void SphereRenderer::initBuffers() {
	// Generate and bind VAO, VBO, and EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Bind and fill VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Bind and fill EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
void SphereRenderer::generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
	vertices.clear();
	indices.clear();

	// Precompute sizes
	unsigned int vertexCount = (stackCount + 1) * (sectorCount + 1);
	unsigned int indexCount = stackCount * sectorCount * 6;

	// Reserve space to prevent multiple reallocations
	vertices.resize(vertexCount * 6); // 3 for position, 3 for normal
	indices.resize(indexCount);

	float x, y, z, xy;
	float nx, ny, nz, lengthInv = 1.0f / radius;
	float sectorStep = 2.0f * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	unsigned int vertexIndex = 0;
	for (unsigned int i = 0; i <= stackCount; ++i) {
		stackAngle = M_PI / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (unsigned int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			// Store vertex position
			vertices[vertexIndex++] = x;
			vertices[vertexIndex++] = y;
			vertices[vertexIndex++] = z;

			// Store normalized vertex normal
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			vertices[vertexIndex++] = nx;
			vertices[vertexIndex++] = ny;
			vertices[vertexIndex++] = nz;
		}
	}

	unsigned int indexIndex = 0;
	for (unsigned int i = 0; i < stackCount; ++i) {
		unsigned int k1 = i * (sectorCount + 1);
		unsigned int k2 = k1 + sectorCount + 1;

		for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices[indexIndex++] = k1;
				indices[indexIndex++] = k2;
				indices[indexIndex++] = k1 + 1;
			}

			if (i != (stackCount - 1)) {
				indices[indexIndex++] = k1 + 1;
				indices[indexIndex++] = k2;
				indices[indexIndex++] = k2 + 1;
			}
		}
	}
}