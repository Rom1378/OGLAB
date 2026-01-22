#pragma once

#include <vector>
#include "CORE/Mesh/Vertex.hpp"
constexpr float M_PI = 3.14159265359f;
namespace MeshRenderer {

	void generateSphere(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, float radius, uint32_t sectorCount, uint32_t stackCount) {
		vertices.clear();
		indices.clear();

		// Precompute sizes
		uint32_t vertexCount = (stackCount + 1) * (sectorCount + 1);
		uint32_t indexCount = stackCount * sectorCount * 6;

		// Reserve space to prevent multiple reallocations
		vertices.resize(vertexCount * 6); // 3 for position, 3 for normal
		indices.resize(indexCount);

		float x, y, z, xy;
		float nx, ny, nz, lengthInv = 1.0f / radius;
		float sectorStep = 2.0f * M_PI / sectorCount;
		float stackStep = M_PI / stackCount;
		float sectorAngle, stackAngle;

		unsigned int vertexIndex = 0;
		for (unsigned int i = 0; i <= stackCount; ++i) {
			stackAngle = M_PI / 2 - i * stackStep;
			xy = radius * cosf(stackAngle);
			z = radius * sinf(stackAngle);

			for (unsigned int j = 0; j <= sectorCount; ++j) {
				sectorAngle = j * sectorStep;

				x = xy * cosf(sectorAngle);
				y = xy * sinf(sectorAngle);

				// Store vertex position

				vertices[vertexIndex++].Position = { x,y,z };

				// Store normalized vertex normal
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				vertices[vertexIndex++].Normal = { nx,ny,nz };
			}
		}

		unsigned int indexIndex = 0;
		for (unsigned int i = 0; i < stackCount; ++i) {
			unsigned int k1 = i * (sectorCount + 1);
			unsigned int k2 = k1 + sectorCount + 1;

			for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
				if (i != 0) {
					indices[indexIndex++] = k1;
					indices[indexIndex++] = k2;
					indices[indexIndex++] = k1 + 1;
				}

				if (i != (stackCount - 1)) {
					indices[indexIndex++] = k1 + 1;
					indices[indexIndex++] = k2;
					indices[indexIndex++] = k2 + 1;
				}
			}
		}
	}

}
