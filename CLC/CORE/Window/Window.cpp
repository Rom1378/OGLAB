#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <iostream>
#include "Input.hpp"
#include <chrono>

namespace Window
{
	namespace Internal
	{
		inline GLFWwindow* m_window = nullptr;
		inline WindowProps m_props;
		GLuint fbo, texture, rbo;
		//opengl scene size 

		float frameBufferX;
		float frameBufferY;
		float frameBufferWidth;
		float frameBufferHeight;

		const uint32_t sizeArrayFps = 20;
		float lastFPS[sizeArrayFps];
		uint32_t indexFPS = 0;
		float lastTime = 0.0f;
		float avgFPS = 0.0f;

		bool viewport_changed = false;

	}

	bool getViewPortChanged()
	{
		return Internal::viewport_changed;
	}


	//get texture for rendering
	GLuint getTexture() {
		return Internal::texture;
		return 1;
	}

	void init(const WindowProps& props)
	{
		Internal::m_props = props;

		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW\n";
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		Internal::m_window = glfwCreateWindow(
			props.width,
			props.height,
			props.title.c_str(),
			nullptr,
			nullptr
		);

		if (!Internal::m_window)
		{
			std::cerr << "Failed to create GLFW window\n";
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(Internal::m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cerr << "Failed to initialize GLAD\n";
			return;
		}

		setVSync(props.vsync);

		// Initialiser ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.MouseDrawCursor = true; // Force ImGui to draw the cursor
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		if (!ImGui_ImplGlfw_InitForOpenGL(Internal::m_window, true)) {
			std::cerr << "ImGui GLFW initialization failed!" << std::endl;
		}

		if (!ImGui_ImplOpenGL3_Init("#version 460")) {
			std::cerr << "ImGui OpenGL initialization failed!" << std::endl;
		}
	}

	void update()
	{
		//get io
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		glfwSwapBuffers(Internal::m_window);
		//GLenum err;
		//while ((err = glGetError()) != GL_NO_ERROR) {
		//	std::cerr << "OpenGL error: " << err << std::endl;
		//}

		//calculate fps
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - Internal::lastTime;
		Internal::lastTime = currentTime;
		Internal::lastFPS[Internal::indexFPS] = 1.0f / deltaTime;
		Internal::indexFPS = (Internal::indexFPS + 1) % Internal::sizeArrayFps;
		Internal::avgFPS = 0.0f;
		for (uint32_t i = 0; i < Internal::sizeArrayFps; i++) {
			Internal::avgFPS += Internal::lastFPS[i];
		}
		Internal::avgFPS /= Internal::sizeArrayFps;

	}

	void clear()
	{
		// Bind our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Window::Internal::fbo);
		
		// Set viewport to match framebuffer size
		glViewport(0, 0, Internal::frameBufferWidth, Internal::frameBufferHeight);
		
		// Clear both color and depth buffers
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
	}

	void shutdown()
	{
		// Nettoyer ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		if (Internal::m_window)
		{
			glfwDestroyWindow(Internal::m_window);
			Internal::m_window = nullptr;
		}
		glfwTerminate();
	}

	bool isOpen()
	{
		return Internal::m_window && !glfwWindowShouldClose(Internal::m_window);
	}

	GLFWwindow* getNativeWindow()
	{
		return Internal::m_window;
	}

	uint32_t getWidth()
	{
		return Internal::m_props.width;
	}

	uint32_t getHeight()
	{
		return Internal::m_props.height;
	}

	bool isVSync()
	{
		return Internal::m_props.vsync;
	}

	void setVSync(bool enabled)
	{
		Internal::m_props.vsync = enabled;
		glfwSwapInterval(enabled ? 1 : 0);
	}

	void CreateFramebuffer(int width, int height) {
		glGenFramebuffers(1, &Internal::fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, Internal::fbo);

		// Create texture to render to
		glGenTextures(1, &Internal::texture);
		glBindTexture(GL_TEXTURE_2D, Internal::texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Internal::texture, 0);

		// Create renderbuffer object for depth and stencil
		glGenRenderbuffers(1, &Internal::rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, Internal::rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Internal::rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		Internal::frameBufferWidth = width;
		Internal::frameBufferHeight = height;

	}

	void bind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Internal::fbo);
	}

	//a function that size the viewport correctly
	//	ImVec2 pos = ImGui::GetCursorScreenPos();
		//ImVec2 avail = ImGui::GetContentRegionAvail();
		//update viewport
		//glViewport((int)pos.x, (int)pos.y, (int)avail.x, (int)avail.y);
	void update_viewport()
	{
		// Get the position and size of the viewport
		// Update the OpenGL viewport
		glViewport(Internal::frameBufferX, Internal::frameBufferY, Internal::frameBufferWidth, Internal::frameBufferHeight);
	}

	// here we unbind our framebuffer
	void unbind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void rescale_framebuffer(float width, float height)
	{
		std::cout << "Rescaling framebuffer to " << width << "x" << height << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, Internal::fbo);

		// Resize the texture
		glBindTexture(GL_TEXTURE_2D, Internal::texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Resize the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, Internal::rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "ERROR::FRAMEBUFFER:: Resized framebuffer is not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Internal::frameBufferWidth = width;
		Internal::frameBufferHeight = height;

		//should update camera data. (aspect ratio and camera center moved)
		




	}

	GLuint getFramebufferTexture()
	{
		return Internal::texture;
	}

	
	WindowProps const* getWindowProps()
	{
		return &Internal::m_props;
	}

	float getFrameBufferWidth()
	{
		return Internal::frameBufferWidth;
	}

	float getFrameBufferHeight()
	{
		return Internal::frameBufferHeight;
	}

	float getAvgFPS()
	{
		return Internal::avgFPS;
	}
	void drawImGuiInterface()
	{
		// Unbind the FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());

		// Start a new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// Make the main docking window fill the entire viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		// Disable padding and borders for the main docking window
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		// Create the main docking window
		ImGui::Begin("Main Docking Window", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground);

		// Create the docking space
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		// End the main docking window
		ImGui::End();

		// Restore the style variables
		ImGui::PopStyleVar(2);

		// Create an ImGui window to display the OpenGL scene
		ImGui::Begin("OpenGL Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		// Get position and size of Opengl Scene
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 avail = ImGui::GetContentRegionAvail();


		//update viewport
		glViewport((int)pos.x, (int)pos.y, (int)avail.x, (int)avail.y);


		if (Internal::viewport_changed) {
			Internal::viewport_changed = false;
		}
		// Update OpenGL viewport to match the ImGui window
		ImVec2 currentSize = ImGui::GetContentRegionAvail();
		static ImVec2 lastSize = ImVec2(0, 0);

		if (currentSize.x != lastSize.x || currentSize.y != lastSize.y) {
			rescale_framebuffer(currentSize.x, currentSize.y);
			lastSize = currentSize;
			Internal::viewport_changed = true;
		}

		// Check if window is hovered and focused
		bool isWindowHovered = ImGui::IsWindowHovered();
		bool isWindowFocused = ImGui::IsWindowFocused();

		// Handle mouse input for the OpenGL scene
		if (isWindowFocused) {
			// Right-click to enable camera control (lock mouse)
			if (isWindowHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				Input::setMouseLocked(true);
			}

			// Press Escape to exit camera control mode
			if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE) && Input::isMouseLocked()) {
				Input::setMouseLocked(false);
			}
		}

		//viewports 
		ImGui::Begin("Viewport");
		ImGui::Text("Viewport size: %.0f x %.0f", avail.x, avail.y);
		//pos
		ImGui::Text("Viewport position: %.0f x %.0f", pos.x, pos.y);
		ImGui::End();


		// Display input state for debugging
		ImGui::Text("Mouse Locked: %s", Input::isMouseLocked() ? "Yes" : "No");
		ImGui::Text("Window Focused: %s", isWindowFocused ? "Yes" : "No");
		ImGui::Text("Window Hovered: %s", isWindowHovered ? "Yes" : "No");

		// Display mouse delta values
		glm::vec2 mouseDelta = Input::getMouseDelta();
		ImGui::Text("Mouse Delta: (%.1f, %.1f)", mouseDelta.x, mouseDelta.y);

		// Display the OpenGL scene
		ImVec2 availSize = ImGui::GetContentRegionAvail();
		ImGui::Image((void*)(intptr_t)Window::getTexture(), availSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();

		//show 10 openl gl textues in a grid with a contrast slider for eact images

		ImGui::Begin("Textures");

		// Display the OpenGL scene
		ImVec2 availSize2 = ImGui::GetContentRegionAvail();
		ImGui::Image((void*)(intptr_t)1, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::Image((void*)(intptr_t)2, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)(intptr_t)3, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::Image((void*)(intptr_t)4, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image((void*)(intptr_t)5, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::Image((void*)(intptr_t)6, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Image((void*)(intptr_t)7, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();
ImGui::Image((void*)(intptr_t)8, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		//contrast control
		ImGui::End();

		// Create another ImGui window for other contddrols
		ImGui::Begin("Menu");
		//show opengl scene fps
		ImGui::Text("FPS: %.1f", Internal::avgFPS);
		//turn on/off polygonem ode
		if (ImGui::Button("Polygon Mode")) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (ImGui::Button("Fill Mode")) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		ImGui::End();


	}

}

