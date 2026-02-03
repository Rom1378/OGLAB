#include "CORE/Systems/Renderer/Renderer.hpp"


#include "CORE/Debug.hpp"
#include "CORE/Window/Window.hpp"
#include "CORE/Scene.hpp"

#include "CORE/Components/Camera.hpp"


//passes
#include "Skybox.hpp"
#include "MeshRenderer.hpp"
#include "LineRenderer.hpp"

#include "ShadowMap.hpp"


class frameBuffer {

	static void init() {

	}

	static void rescale(float width, float height) {
	}
};

namespace Renderer {
	GLuint gFrameBuffer = {};
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



	void init() {

		// Framebuffer init
		glGenFramebuffers(1, &gFrameBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer);


		//color buffer
		glGenTextures(1, &gTexColorBuffer);
		glBindTexture(GL_TEXTURE_2D, gTexColorBuffer);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA, Window::getFrameBufferWidth(), Window::getFrameBufferHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gTexColorBuffer, 0);


		//rbo depth + stencil
		glGenRenderbuffers(1, &gRboDepthStencil);
		glBindRenderbuffer(GL_RENDERBUFFER, gRboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRboDepthStencil);

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



		Skybox::init();
		MeshRenderer::init();
		LineRenderer::init();

		ShadowMapSystem::init();

		
		LOG_OK("Skybox initialized");
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

		// Resize the texture
		glBindTexture(GL_TEXTURE_2D, gTexColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Resize the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, gRboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG_ERROR("ERROR::FRAMEBUFFER:: Resized framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}


	GLuint getMainFBO() { return gFrameBuffer; }
	GLuint getMainRBO() { return gRboDepthStencil; }
	GLuint getColorTextureBuffer() { return gTexColorBuffer; }


	void setViewProjection(const glm::mat4& view, const glm::mat4& proj) {
		renderCam.view = view;
		renderCam.projection = proj;
	}

	const glm::mat4& getView() { return renderCam.view; }
	const glm::mat4& getProjection() { return renderCam.projection; }




}
