#include "PostProcessing.hpp"

#include "Renderer.hpp"
#include "CORE/Mesh/Quad.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Debug.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "CORE/Window/Window.hpp"

namespace PostProcessing
{
	bool gammaCorrectionEnabled = true;
	float gammaCorrectionValue = 2.2f;

	//quad for post processing
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	GLuint quadEBO = 0;


	//GLuint gPostProcessFBO = {};
	//GLuint gPosProcessColorBuffer = {};


	void init() {

		// Framebuffer init
		/*
		glGenFramebuffers(1, &gPostProcessFBO);

		glBindFramebuffer(GL_FRAMEBUFFER, gPostProcessFBO);


		//color buffer
		glGenTextures(1, &gPosProcessColorBuffer);
		glBindTexture(GL_TEXTURE_2D, gPosProcessColorBuffer);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA, Window::getFrameBufferWidth(), Window::getFrameBufferHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosProcessColorBuffer, 0);


		//rbo depth + stencil
		//glGenRenderbuffers(1, &gRboDepthStencil);
		//glBindRenderbuffer(GL_RENDERBUFFER, gRboDepthStencil);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());

		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRboDepthStencil);

		// In init() function, add this check:
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			// Check specific error
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			// Log status for debugging
			LOG_ERROR("Framebuffer is not complete! status: ", status);
		}
		else {
			LOG_OK("Created FBO + texture and RBO");
		}
*/






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

	}

	void renderPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::getMainFBO());


		if (!gammaCorrectionEnabled)
			return;

		auto shader = ShaderManager::getShader("gammaCorrection");
		if (!shader) {
			LOG_ERROR("Failed to get postprocess shader");
			return;
		}


		glDisable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		shader->use();
		glBindVertexArray(quadVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Renderer::getColorTextureBuffer());

		shader->setInt("screenTexture", 0);
		shader->setFloat("gammaCorrection", gammaCorrectionValue);


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f));
		shader->setMat4("model", model);

		glDrawElements(GL_TRIANGLES, MeshRawData::Quad::indices.size(), GL_UNSIGNED_INT, 0);

		//glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}


	void gammaCorrectionPass() {

	}

	float getGamma() { return gammaCorrectionValue; }
	void setGamma(float gamma) { gammaCorrectionValue = gamma; }
	void enableGammaCorrection(bool enabled) { gammaCorrectionEnabled = enabled; }
	bool isGammaCorrectionEnabled() { return gammaCorrectionEnabled; }

}