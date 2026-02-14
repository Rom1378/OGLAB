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
	
	// Bloom
	float bloomThreshold = 0.5f;
	float bloomIntensity = 1.0f;
	int bloomBlurPasses = 10;
	GLuint pingpongFBO[2];
	GLuint pingpongColorbuffers[2];
	
	// Quad pour post-processing
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	GLuint quadEBO = 0;

	void init() {
		uint32_t width = Window::getFrameBufferWidth();
		uint32_t height = Window::getFrameBufferHeight();
		
		// Ping pong framebuffer pour le blur du bloom
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorbuffers);
		
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				LOG_ERROR("Ping-pong FBO[", i, "] not complete!");
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Setup quad VAO
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
		
		LOG_OK("PostProcessing initialized (HDR + Tone Mapping + Gamma + Bloom)");
	}

	void renderPass() {
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(quadVAO);
		
		// Blur le bloom avec ping-pong
		auto blurShader = ShaderManager::getShader("blur"); 
		if (blurShader) {
			blurShader->use();
			
			bool horizontal = true;
			bool first_iteration = true;
			
			for (unsigned int i = 0; i < bloomBlurPasses; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				
				blurShader->setBool("horizontal", horizontal);
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? Renderer::getBrightnessTextureBuffer() : pingpongColorbuffers[!horizontal]);
				
				blurShader->setInt("image", 0);
				
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::scale(model, glm::vec3(2.0f));
				blurShader->setMat4("model", model);
				
				glDrawElements(GL_TRIANGLES, MeshRawData::Quad::indices.size(), GL_UNSIGNED_INT, 0);
				
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
		}
		else {
			LOG_ERROR("Blur shader not found! Skipping bloom blur.");
		}
		
		// Tone mapping + Composition finale
		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::getPostProcessedFBO());
		glViewport(0, 0, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		auto tonemapShader = ShaderManager::getShader("tonemap");
		if (!tonemapShader) {
			LOG_ERROR("Failed to get tonemap shader");
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			return;
		}
		
		tonemapShader->use();
		
		// Bind HDR color buffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Renderer::getColorTextureBuffer());
		
		// Bind bloom blur result
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]);
		
		// Uniforms
		tonemapShader->setInt("hdrBuffer", 0);
		tonemapShader->setInt("bloomBlur", 1);
		tonemapShader->setFloat("exposure", exposure);
		tonemapShader->setFloat("gamma", gammaCorrectionValue);
		tonemapShader->setFloat("bloomIntensity", bloomIntensity);
		tonemapShader->setBool("hdrEnabled", hdrEnabled);
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f));
		tonemapShader->setMat4("model", model);
		
		glDrawElements(GL_TRIANGLES, MeshRawData::Quad::indices.size(), GL_UNSIGNED_INT, 0);
		
		Renderer::increaseDrawCallCount();
		
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
	}
	
	void rescale(float width, float height) {
		if (width <= 0 || height <= 0) {
			LOG_WARN("Invalid dimensions for PostProcessing rescale: ", width, "x", height);
			return;
		}
		
		// Redimensionner les ping-pong buffers
		for (unsigned int i = 0; i < 2; i++) {
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_FLOAT, NULL);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	// Getters/Setters
	float getGamma() { return gammaCorrectionValue; }
	void setGamma(float gamma) { gammaCorrectionValue = gamma; }
	
	float getExposure() { return exposure; }
	void setExposure(float exp) { exposure = exp; }
	
	void enableHDR(bool enabled) { hdrEnabled = enabled; }
	bool isHDREnabled() { return hdrEnabled; }
	
	float getBloomThreshold() { return bloomThreshold; }
	void setBloomThreshold(float threshold) { bloomThreshold = threshold; }
	
	float getBloomIntensity() { return bloomIntensity; }
	void setBloomIntensity(float intensity) { bloomIntensity = intensity; }
	
	int getBloomBlurPasses() { return bloomBlurPasses; }
	void setBloomBlurPasses(int passes) { bloomBlurPasses = passes; }
}