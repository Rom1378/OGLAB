#pragma once

#include <memory>
#include <vector>

#include "../GameObject.hpp"

class Prefab : public GameObject
{
public:
	Prefab() = default;
	virtual ~Prefab() = default;

	virtual void init() = 0;
	virtual void draw() = 0;
	virtual void update(float deltaTime) = 0;

	void addChild(std::shared_ptr<GameObject> child) {
		children.push_back(child);
	}

	void removeChild(std::shared_ptr<GameObject> child) {
		children.erase(std::remove(children.begin(), children.end(), child), children.end());
	}

	void setParent(std::shared_ptr<GameObject> parent) {
		this->parent = parent;
	}

	std::shared_ptr<GameObject> getParent() {
		return parent;
	}

	std::vector<std::shared_ptr<GameObject>> getChildren() {
		return children;
	}	


}; // class Prefab