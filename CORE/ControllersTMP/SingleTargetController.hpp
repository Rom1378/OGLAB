/*#pragma once

class GameObject;

class SingleTargetController {

public:
	SingleTargetController(const std::shared_ptr<GameObject>& obj) : m_targetObj{obj} {}
	virtual ~SingleTargetController() {}
	virtual void update(float dt) = 0;

	std::shared_ptr<GameObject> get_target_object() const { return m_targetObj; }
	void set_target_object(std::shared_ptr<GameObject> go) { m_targetObj = go; }

private:
	std::shared_ptr<GameObject> m_targetObj;

};
*/