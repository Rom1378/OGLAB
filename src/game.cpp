#include "game.hpp"
#include "CORE/Entity.hpp"

#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Mesh.hpp"

Game::Game() : Scene(), player{}, terrain{} {}

void Game::init() {
	player = createEntity();
	auto playerTransform = addComponent<Transform>(player, glm::vec3(1, 1, 1));

	terrain = createEntity();
	addComponent<Transform>(terrain, glm::vec3(1, -10, 1));

}

void Game::onUpdate() {

}
