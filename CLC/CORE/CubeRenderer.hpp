#pragma once
#include "RenderComponent.hpp"
#include "GameObject.hpp"
#include <iostream>

class CubeRenderer : public RenderComponent {

protected:
	

public:


	void init() override;
	void draw(const std::shared_ptr<Camera> cam) override;
	~CubeRenderer() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

};


