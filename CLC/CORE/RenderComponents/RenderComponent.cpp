#include "RenderComponent.hpp"


void RenderComponent::setRenderer(std::shared_ptr<InterfaceRenderer> renderer)
{
	m_renderer = renderer;
	if (m_renderer)
	{
		m_renderer->initialize(this);
	}
}

void RenderComponent::addTexture(const std::string& textureName)
{
	auto texture = TextureManager::getTexture(textureName);
	if (texture) {
		m_textures.push_back(texture);
	}
	else {
		std::cerr << "Failed to load texture: " << textureName << std::endl;
	}
}

void RenderComponent::bindTextures()
{
	for (unsigned int i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]->id);
		std::string uniformName = "texture" + std::to_string(i + 1);
		m_shader->setInt(uniformName, i);
	}
}

void RenderComponent::unBindTextures() {
	for (unsigned int i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}