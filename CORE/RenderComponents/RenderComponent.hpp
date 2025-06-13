#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../Component.hpp"
#include "../Transform.hpp"
#include "InterfaceRenderer.hpp"
#include "../Shader.hpp"
#include "../TextureManager.hpp"
#include <iostream>
#include "../Cameras/Camera.hpp"

class RenderComponent : public Component {
public:
	RenderComponent() : Component(), m_color(glm::vec4(1, 1, 1, 1)), m_renderer(nullptr), m_shader(nullptr),
		VAO(0), VBO(0), EBO(0), m_isShadowCaster(false), m_isShadowReceiver(false) {	}
	RenderComponent(bool isShadowCaster, bool isShadowReceiver) : Component(), m_color(glm::vec4(1, 1, 1, 1)), m_renderer(nullptr), m_shader(nullptr),
		VAO(0), VBO(0), EBO(0), m_isShadowCaster(isShadowCaster), m_isShadowReceiver(isShadowReceiver) {	}
	RenderComponent(const glm::vec4& color) : Component(), m_color(color), m_renderer(nullptr), m_shader(nullptr),
		VAO(0), VBO(0), EBO(0), m_isShadowCaster(false), m_isShadowReceiver(false) {	}
	RenderComponent(const glm::vec4& color, bool isShadowCaster, bool isShadowReceiver) : Component(), m_color(color), m_renderer(nullptr), m_shader(nullptr),
		VAO(0), VBO(0), EBO(0), m_isShadowCaster(isShadowCaster), m_isShadowReceiver(isShadowReceiver) {	}

	virtual void draw(const glm::mat4& view, const glm::mat4& projection) {};
	virtual void draw(const std::shared_ptr<Camera> cam) {};
	virtual void draw() {};
	virtual void renderRawGeometry(const glm::mat4& lightSpaceMatrix) {};// Shadow pass
	virtual void renderWithMaterials(const std::shared_ptr<Camera>& cam) {}; // Main pass

	bool getIsShadowCaster() const { return m_isShadowCaster; }
	bool getIsShadowReceiver() const { return m_isShadowReceiver; }
	void setIsShadowCaster(bool isShadowCaster) { m_isShadowCaster = isShadowCaster; }
	void setIsShadowReceiver(bool isShadowReceiver) { m_isShadowReceiver = isShadowReceiver; }

	void setRenderer(std::shared_ptr<InterfaceRenderer> renderer);
	void setShader(const std::string& shaderName)	{		m_shader = ShaderManager::getShader(shaderName);	}
	void setColor(const glm::vec3& color)	{		m_color = glm::vec4(color, 1.0f);	}
	void setColor(const glm::vec4& color) {		m_color = color;	}
	glm::vec4 getColor() const	{		return m_color;	}
	void addTexture(const std::string& textureName);
	void addTexture(std::shared_ptr<Texture> texture)	{		m_textures.push_back(texture);	}
	void bindTextures();
	void unBindTextures();
	void setShader(std::shared_ptr<ShaderProgram> shader)	{		m_shader = shader;	}
	std::shared_ptr<ShaderProgram> getShader() const {		return m_shader;	}


protected:

	std::shared_ptr<InterfaceRenderer> m_renderer;
	std::shared_ptr<ShaderProgram> m_shader;
	glm::vec4 m_color;

	unsigned int VAO, VBO, EBO;

	std::vector<std::shared_ptr<Texture>> m_textures;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	bool m_isShadowCaster;
	bool m_isShadowReceiver;

}; // class RenderComponent

