#pragma once

#include "../RenderComponent.hpp"
#include "../Window.hpp"

class UICursorComponent : public RenderComponent {
public:

	UICursorComponent() : RenderComponent() {
		init();
	}
	UICursorComponent(const UICursorComponent& other) : RenderComponent(other) {
		init();
	}
	void init() override {
		vertices = { -10.0f,  0.0f,  // Horizontal line
	 10.0f,  0.0f,
	 0.0f, -10.0f,  // Vertical line
	 0.0f,  10.0f };
		indices = {};


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		setShader("cursor");
	}

	void draw(const std::shared_ptr<Camera> cam) override {
		draw();
	}
	void draw() override {

		auto shader = ShaderManager::getShader("cursor");
		if (!shader) return;

		shader->use();

		glDisable(GL_DEPTH_TEST);  // UI elements should render on top

		// Compute orthographic projection
		glm::mat4 projection = glm::ortho(0.0f, (float)Window::getFrameBufferWidth(), 0.0f, (float)Window::getFrameBufferHeight());
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(Window::getFrameBufferWidth() / 2.0f, Window::getFrameBufferHeight() / 2.0f, 0.0f));

		// Use shader program

		// Send matrices to shader
		GLuint projLoc = glGetUniformLocation(shader->getProgram(), "uProjection");
		GLuint modelLoc = glGetUniformLocation(shader->getProgram(), "uModel");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Draw crosshair
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 4);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
	}
	void draw(const glm::mat4& view, const glm::mat4& projection) override {
		draw();
	}
};