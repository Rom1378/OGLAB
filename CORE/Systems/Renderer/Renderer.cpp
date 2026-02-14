#include "CORE/Systems/Renderer/Renderer.hpp"


#include "CORE/Debug.hpp"
#include "CORE/Window/Window.hpp"
#include "CORE/Scene.hpp"

#include "CORE/Components/Camera.hpp"


//passes
#include "Skybox.hpp"
#include "MeshRenderer.hpp"
#include "LineRenderer.hpp"
#include "PostProcessing.hpp"

#include "ShadowMap.hpp"


namespace Renderer {

	GLuint gFrameBuffer = {}, gPostProcessedFBO = {};
	GLuint gColorBrightnessThresholdBuffer = 0;
	GLuint gPostProcessedTexture = {};

	GLuint gTexColorBuffer = {};
	GLuint gRboDepthStencil = {};


	struct RenderCamera {
		glm::mat4 projection{};
		glm::mat4 view{};
	} renderCam;

	glm::vec3 getViewPosition() {
		return glm::vec3(glm::inverse(renderCam.view)[3]);
	}

	RendererSettings rdSettings{};

	RendererSettings& getRendererSettings() {
		return rdSettings;
	}

	uint32_t drawCallCount = 0;



	void init() {
		// Framebuffer init
		glGenFramebuffers(1, &gFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer);

		// Color buffer - USE GL_RGBA16F for linear HDR rendering
		glGenTextures(1, &gTexColorBuffer);
		glGenTextures(1, &gColorBrightnessThresholdBuffer);

		glBindTexture(GL_TEXTURE_2D, gTexColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::getFrameBufferWidth(), Window::getFrameBufferHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, MAIN_COLOR_ATTACHMENT, GL_TEXTURE_2D, gTexColorBuffer, 0);


		glBindTexture(GL_TEXTURE_2D, gColorBrightnessThresholdBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::getFrameBufferWidth(), Window::getFrameBufferHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, BRIGHTNESS_THRESHOLD_ATTACHMENT, GL_TEXTURE_2D, gColorBrightnessThresholdBuffer, 0);


		// RBO depth + stencil
		glGenRenderbuffers(1, &gRboDepthStencil);
		glBindRenderbuffer(GL_RENDERBUFFER, gRboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRboDepthStencil);



		unsigned int attachments[2] = { MAIN_COLOR_ATTACHMENT, BRIGHTNESS_THRESHOLD_ATTACHMENT };
		glDrawBuffers(2, attachments);


	


		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			LOG_ERROR("Framebuffer is not complete! status: ", status);
		}
		else {
			LOG_OK("Created FBO + texture (GL_RGBA16F linear) and RBO");
		}


		glGenFramebuffers(1, &gPostProcessedFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, gPostProcessedFBO);

		glGenTextures(1, &gPostProcessedTexture);
		glBindTexture(GL_TEXTURE_2D, gPostProcessedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Window::getFrameBufferWidth(), Window::getFrameBufferHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPostProcessedTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG_ERROR("Post-processed framebuffer is not complete!");
		}



		Skybox::init();
		MeshRenderer::init();
		LineRenderer::init();
		ShadowMapSystem::init();
		PostProcessing::init();

		LOG_OK("Renderer initialized with linear HDR framebuffer");
	}

	void update(Scene& scene, float dt) {
		ShadowMapSystem::update(scene);

	}

	void prerender(Scene* scene) {
		ShadowMapSystem::computeShadowMapsPass(*scene);
	}


	void render(Scene* scene) {
		//shadow 
		//prerender(scene);

		drawCallCount = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer);
		clearFBO();


		if (scene->hasSkybox()) {
			Skybox::skyboxPass(scene);
		}

		MeshRenderer::renderPass(scene);
		LineRenderer::render();


		//draw each pass.
		// cubepass
		//modelPass
		//linePass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void postRender() {
		PostProcessing::renderPass();
	}



	void clearFBO() {

		// Set viewport to match framebuffer size
		//TODO THIS NEEDS TO GO IN RESIZE MAYBE

		//Window::clear();
		Window::update_viewport();
		//glViewport(0, 0, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());

		// Clear both color and depth buffers
		glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable depth testing
		glEnable(GL_DEPTH_TEST);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);        // Ne pas rendre les faces arrière
		//glFrontFace(GL_CCW);
	}


	void deleteBuffers() {
		glDeleteTextures(1, &gTexColorBuffer);
		glDeleteRenderbuffers(1, &gRboDepthStencil);
		glDeleteFramebuffers(1, &gFrameBuffer);
	}

	void cleanup() {
		deleteBuffers();
		Skybox::cleanup();
		MeshRenderer::cleanup();
		LineRenderer::cleanup();

	}

	void rescale_framebuffer(float width, float height) {
		LOG_OK("Rescaling FBO");

		glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer);

		// Resize the main color texture - KEEP GL_RGBA16F format
		glBindTexture(GL_TEXTURE_2D, gTexColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Resize the brightness threshold texture
		glBindTexture(GL_TEXTURE_2D, gColorBrightnessThresholdBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		// Resize the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, gRboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Resize post-processed texture
		glBindTexture(GL_TEXTURE_2D, gPostProcessedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);


		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG_ERROR("ERROR::FRAMEBUFFER:: Resized framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		PostProcessing::rescale(width, height);
	}


	GLuint getMainFBO() { return gFrameBuffer; }
	GLuint getMainRBO() { return gRboDepthStencil; }
	GLuint getColorTextureBuffer() { return gTexColorBuffer; }

	GLuint getPostProcessedFBO() { return gPostProcessedFBO; }
	GLuint getPostProcessedTexture() { return gPostProcessedTexture; }

	GLuint getBrightnessTextureBuffer() { return gColorBrightnessThresholdBuffer; }


	void setViewProjection(const glm::mat4& view, const glm::mat4& proj) {
		renderCam.view = view;
		renderCam.projection = proj;
	}

	const glm::mat4& getView() { return renderCam.view; }
	const glm::mat4& getProjection() { return renderCam.projection; }


	uint32_t getDrawCallCount() { return drawCallCount; }
	void increaseDrawCallCount() { drawCallCount++; }


}
