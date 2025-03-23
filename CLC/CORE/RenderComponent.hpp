#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Component.hpp"
#include "Transform.hpp"
#include "InterfaceRenderer.hpp"
#include "Shader.hpp"
#include "TextureManager.hpp"
#include <iostream>


class RenderComponent : public Component
{


public:
	virtual void draw(const glm::mat4& view, const glm::mat4& projection) = 0;
	virtual void draw()  {};

	void setRenderer(std::shared_ptr<InterfaceRenderer> renderer)
	{
		m_renderer = renderer;
		if (m_renderer)
		{
			m_renderer->initialize(this);
		}
	}

	void setShader(const std::string& shaderName)
	{
		m_shader = ShaderManager::getShader(shaderName);

	}

	void setColor(const glm::vec3& color)
	{
		m_color = color;
	}
	glm::vec3 getColor() const
	{
		return m_color;
	}

	void addTexture(const std::string& textureName)
	{
		auto texture = TextureManager::getTexture(textureName);
		if (texture) {
			m_textures.push_back(texture);
		}
		else {
			std::cerr << "Failed to load texture: " << textureName << std::endl;
		}
	}

	void addTexture(std::shared_ptr<Texture> texture)
	{
		m_textures.push_back(texture);
	}

	void bindTextures()
	{
		for (unsigned int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i]->id);
			std::string uniformName = "texture" + std::to_string(i + 1);
			m_shader->setInt(uniformName, i);
		}
	}

	void unBindTextures() {
		for (unsigned int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}


protected:

	std::shared_ptr<InterfaceRenderer> m_renderer;
	std::shared_ptr<ShaderProgram> m_shader;
	//color
	glm::vec3 m_color;

	unsigned int VAO, VBO, EBO;

	std::vector<std::shared_ptr<Texture>> m_textures;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;




}; // class RenderComponent