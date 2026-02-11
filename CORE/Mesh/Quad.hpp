#pragma once
#include <vector>
#include <cstdint>
#include "Vertex.hpp"

namespace MeshRawData {
	namespace Quad {
		const std::vector<Vertex> vertices = {
			// Positions        // Normals         // Texture Coords
			Vertex({ -0.5f, -0.5f, 0.0f }, { 0.0f,  0.0f, 1.0f }, { 0.0f,  0.0f }),
			Vertex({ 0.5f, -0.5f, 0.0f }, { 0.0f,  0.0f, 1.0f }, { 1.0f,  0.0f }),
			Vertex({ 0.5f,  0.5f, 0.0f }, { 0.0f,  0.0f, 1.0f }, { 1.0f,  1.0f }),
			Vertex({ -0.5f,  0.5f, 0.0f }, { 0.0f,  0.0f, 1.0f }, { 0.0f,  1.0f })
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2,
			0, 2, 3
		};
	}
}