#pragma once
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	//glm::vec3 Tangent;
	//glm::vec3 Bitangent;
};
namespace MeshRawData {
	namespace Cube
	{
		
		//define float array for cube vertices
		const std::vector<Vertex> vertices = {
			// Positions        // Normals         // Texture Coords
		   Vertex({ -0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 0.0f,  0.0f }),
Vertex({0.5f, -0.5f, -0.5f},  {0.0f,  0.0f, -1.0f }, {1.0f, 0.0f }) ,
Vertex({0.5f,  0.5f, -0.5f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}),
Vertex({-0.5f,  0.5f, -0.5f},  {0.0f,  0.0f, -1.0f},  {0.0f,  1.0f}),
Vertex({-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}),
Vertex({0.5f, -0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f,  0.0f}),
Vertex({0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}),
Vertex({-0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {0.0f,  1.0f}),
Vertex({-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}),
Vertex({-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}),
Vertex({-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}),
Vertex({-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}),
Vertex({0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}),
Vertex({0.5f,  0.5f, -0.5f},  {1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}),
Vertex({0.5f, -0.5f, -0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}),
Vertex({0.5f, -0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}),
Vertex({-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}),
Vertex({0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},  {1.0f,  1.0f}),
Vertex({0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}),
Vertex({-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},  {0.0f,  0.0f}),
Vertex({-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}),
Vertex({0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f,  1.0f}),
Vertex({0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}),
Vertex({-0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f,  0.0f}) };

		// Indices for drawing the cube faces
		std::vector<uint32_t> indices = {
			0, 1, 2,  0, 2, 3,
			4, 5, 6,  4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23 };

	}
}
