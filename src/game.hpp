#pragma once

#include "CORE/Scene.hpp"

class Game : public Scene {
	Game();
	void init() override;

	void onUpdate() override;
private:

	Entity player;
	Entity terrain;
};