#include "LineRenderer.hpp"
#include "CameraComponents/Camera.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


namespace LineRenderer {

	GLuint VAO, VBO, EBO;
	uint32_t allocatedLines = 128;
	uint32_t count = 0;

	struct line_data {
		glm::vec3 start;
		glm::vec3 end;
		glm::vec4 color;
	};
	struct line_vertex {
		glm::vec3 point;
		glm::vec4 color;
	};

	std::vector<line_vertex> data = {};

	void init() {

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, allocatedLines * sizeof(line_vertex)*2, nullptr, GL_DYNAMIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(line_vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(line_vertex), (void*)(3 * sizeof(float)));

		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(line_data), (void*)(6 * sizeof(float)));

		glBindVertexArray(0);
	}

	void draw(glm::vec3 const& start, glm::vec3 const& end, glm::vec4 const& color) {
		line_data newLine = { start, end, color };

		line_vertex startv = { start, color };
		line_vertex endv = { end, color };



		data.push_back(startv);
		data.push_back(endv);

		count += 1;
	}

	void LineRenderer::render(const std::shared_ptr<CameraComponent> cam) {

		glBindVertexArray(VAO);

		if (count > allocatedLines) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			allocatedLines = count + 128;
			glBufferData(GL_ARRAY_BUFFER, allocatedLines * sizeof(line_vertex)*2, &data[0], GL_DYNAMIC_DRAW);
		}
		else if (count > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(line_vertex)*2, &data[0]);
		}

		auto lineShader = ShaderManager::getShader("line");

		lineShader->use();

		// Set matrix uniforms
		lineShader->setMat4("model", glm::mat4(1.0f));
		lineShader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
		lineShader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, count * 2);
		data.clear();
		count = 0;

	}

}