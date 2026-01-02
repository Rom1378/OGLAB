#include "PhysicsScene.hpp"

#include "LineRenderer.hpp"




void PhysicsScene::drawDebugVisualization() {
	if (m_scene) {
		const PxRenderBuffer& renderBuffer = m_scene->getRenderBuffer();
		for (PxU32 i = 0; i < renderBuffer.getNbLines(); ++i) {
			const PxDebugLine& line = renderBuffer.getLines()[i];
			glm::vec3 start(line.pos0.x, line.pos0.y, line.pos0.z);
			glm::vec3 end(line.pos1.x, line.pos1.y, line.pos1.z);
			glm::vec4 color(0, 1, 0, 1);
			// Render the line using your rendering engine
			LineRenderer::draw(start, end, color);
		}
	}
}

void PhysicsScene::renderDebugVisualization(const std::shared_ptr<CameraComponent> cam) {
	LineRenderer::render(cam);
}