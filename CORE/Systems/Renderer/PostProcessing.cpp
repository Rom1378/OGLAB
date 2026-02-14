#include "PostProcessing.hpp"

#include "Renderer.hpp"
#include "CORE/Mesh/Quad.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Debug.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "CORE/Window/Window.hpp"

namespace PostProcessing
{
	// HDR + Tone Mapping
	bool hdrEnabled = true;
	float exposure = 1.0f;
	
	// Gamma Correction
	float gammaCorrectionValue = 2.2f;

	// Quad pour post-processing
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	GLuint quadEBO = 0;

	void init() {
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glGenBuffers(1, &quadEBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

		glBufferData(GL_ARRAY_BUFFER, MeshRawData::Quad::vertices.size() * sizeof(Vertex), MeshRawData::Quad::vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MeshRawData::Quad::indices.size() * sizeof(uint32_t), MeshRawData::Quad::indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

		glBindVertexArray(0);
		
		LOG_OK("PostProcessing initialized (HDR + Tone Mapping + Gamma)");
	}

	void renderPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::getMainFBO());
		
		// Utiliser le shader de tone mapping unifié
		auto shader = ShaderManager::getShader("tonemap");
		if (!shader) {
			LOG_ERROR("Failed to get tonemap shader");
			return;
		}

		glDisable(GL_DEPTH_TEST);
		
		shader->use();
		glBindVertexArray(quadVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Renderer::getColorTextureBuffer());

		// Uniforms
		shader->setInt("hdrBuffer", 0);
		shader->setFloat("exposure", exposure);
		shader->setFloat("gamma", gammaCorrectionValue);
		shader->setBool("hdrEnabled", hdrEnabled);

		// Modèle pour remplir l'écran
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f));
		shader->setMat4("model", model);

		glDrawElements(GL_TRIANGLES, MeshRawData::Quad::indices.size(), GL_UNSIGNED_INT, 0);

		Renderer::increaseDrawCallCount();

		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Getters/Setters
	float getGamma() { return gammaCorrectionValue; }
	void setGamma(float gamma) { gammaCorrectionValue = gamma; }
	
	float getExposure() { return exposure; }
	void setExposure(float exp) { exposure = exp; }
	
	void enableHDR(bool enabled) { hdrEnabled = enabled; }
	bool isHDREnabled() { return hdrEnabled; }
	
}