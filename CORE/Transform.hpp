#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4
#include <PxPhysics.h>


struct Transform {
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;

	Transform(const glm::vec3& pos = glm::vec3(0.0f),
		const glm::quat& rot = glm::quat(1, 0, 0, 0),
		const glm::vec3& scale = glm::vec3(1.0f))
		: pos(pos), rot(rot), scale(scale) {
	}
	Transform(const Transform&) = default;

	glm::mat4 getViewMatrix() const {
		glm::vec3 forward = rot * glm::vec3(0, 0, -1);  // Forward is -Z in view space
		glm::vec3 up = rot * glm::vec3(0, 1, 0);
		glm::vec3 right = rot * glm::vec3(1, 0, 0);

		glm::vec3 target = pos + forward;

		return glm::lookAt(pos, target, up);
	}

	glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pos);

		// Use quaternion for rotation instead of Euler angles
		model *= glm::toMat4(rot);

		model = glm::scale(model, scale);
		return model;
	}

	glm::mat4 toMat4() const {
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, pos);
		transform = transform * glm::mat4_cast(rot);
		return transform;
	}


	Transform operator*(const Transform& other) const {
		Transform result;
		result.pos = pos + (rot * (scale * other.pos));
		result.rot = rot * other.rot;
		result.scale = scale * other.scale;
		return result;
	}

	Transform inverse() const {
		Transform inv;
		inv.rot = glm::inverse(rot);
		inv.scale = 1.0f / scale; // Component-wise division
		inv.pos = inv.rot * (-pos / scale);
		return inv;
	}

	void fromPx(const physx::PxTransform& t) {
		pos = glm::vec3(t.p.x, t.p.y, t.p.z);
		//glm::quat newRot = glm::quat(t.q.x, t.q.y, t.q.z, t.q.w);
		glm::quat newRot = glm::quat(t.q.w, t.q.x, t.q.y, t.q.z);

		//if (glm::dot(rot, rot) < 0.0f)
		if (glm::dot(rot, newRot) < 0.0f)
			newRot = -newRot;

		rot = glm::normalize(newRot);
	}

	physx::PxTransform toPx() const {
		return physx::PxTransform(
			physx::PxVec3(pos.x, pos.y, pos.z),
			physx::PxQuat(rot.x, rot.y, rot.z, rot.w)
		);
	}


	void rotate(float pitch, float yaw, float roll) {
		glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat rollQuat = glm::angleAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f));
		rot = yawQuat * pitchQuat * rollQuat * rot;
		rot = glm::normalize(rot);
	}


	/*
	//http://www.gamedev.net/forums/topic/56471-extracting-direction-vectors-from-quaternion/1273785
	glm::vec3 forward() const {
		float x{ rot.x }, y{ rot.y }, z{ rot.z }, w{ rot.w };
		return glm::vec3{ 2 * (x * z + w * y) , 2 * (y * z - w * x),1 - 2 * (x * x + y * y) };
	}
	glm::vec3 up() const {
		float x{ rot.x }, y{ rot.y }, z{ rot.z }, w{ rot.w };
		return glm::vec3{ 2 * (x * y - w * z),1 - 2 * (x * x + z * z),2 * (y * z + w * x) };
	}
	glm::vec3 left() const {
		float x{ rot.x }, y{ rot.y }, z{ rot.z }, w{ rot.w };
		return glm::vec3{ 1 - 2 * (y * y + z * z), 2 * (x * y + w * z),2 * (x * z - w * y) };
	}*/


	glm::vec3 forward() const {
		return rot * glm::vec3(0, 0, -1);
	}

	glm::vec3 up() const {
		return rot * glm::vec3(0, 1, 0);
	}

	glm::vec3 right() const {
		return rot * glm::vec3(1, 0, 0);
	}


	glm::vec3 getEulerAngles() const {
		return glm::degrees(glm::eulerAngles(rot));
	}

	void setEulerAngles(const glm::vec3& euler) {
		rot = glm::quat(glm::radians(euler));
	}

};

/*
class Transform2 {

public:
	Transform2(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
		: m_position(position), m_rotation(rotation), m_scale(scale) {
		updateQuaternionFromEuler();
	}

	const glm::vec3& getPosition() const { return m_position; }
	const glm::vec3& getRotation() const { return m_rotation; }
	const glm::quat& getRotationQuaternion() const { return m_rotationQuat; }
	const glm::vec3& getScale() const { return m_scale; }

	void setPosition(const glm::vec3& position) { m_position = position; }
	void setRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		updateQuaternionFromEuler();
	}
	void setRotationQuaternion(const glm::quat& quaternion) {
		m_rotationQuat = quaternion;
		// Update Euler angles for compatibility
		m_rotation = glm::degrees(glm::eulerAngles(glm::normalize(m_rotationQuat)));
	}

	void setScale(const glm::vec3& scale) { m_scale = scale; }

	virtual void move(const glm::vec3& offset) { m_position += offset; }
	//void rotate(const glm::vec3& offset) { m_rotation += offset; }

	void rotate(const glm::vec3& offset) {
		m_rotation += offset;
		updateQuaternionFromEuler();
	}
	void scale(const glm::vec3& offset) { m_scale += offset; }

	glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);

		// Use quaternion for rotation instead of Euler angles
		model *= glm::toMat4(m_rotationQuat);

		model = glm::scale(model, m_scale);
		return model;
	}

protected:

	void updateQuaternionFromEuler() {
		m_rotationQuat = glm::quat(glm::radians(m_rotation));
	}

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::quat m_rotationQuat; // Store rotation as quaternion
	glm::vec3 m_scale;

}; // class Transform


*/
