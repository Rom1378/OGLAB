#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Component.hpp"
#include "Transform.hpp"
#include "InterfaceRenderer.hpp"
#include "Shader.hpp"


class RenderComponent : public Component
{

public:
	virtual void draw() = 0;

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
		m_shader = ShaderManager::get()->getShader(shaderName);

	}
private:

	std::shared_ptr<InterfaceRenderer> m_renderer;
	std::shared_ptr<ShaderProgram> m_shader;


}; // class RenderComponent