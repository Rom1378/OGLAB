#pragma once

#include <vector>
#include "CORE/Mesh/Vertex.hpp"
constexpr float M_PI = 3.14159265359f;
namespace MeshRenderer {
	void generateSphere(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, float radius, uint32_t sectorCount, uint32_t stackCount) {
		vertices.clear();
		indices.clear();

		const uint32_t vertexCount = (stackCount + 1) * (sectorCount + 1);
		const uint32_t indexCount = stackCount * sectorCount * 6;

		vertices.resize(vertexCount);
		indices.resize(indexCount);

		const float sectorStep = 2.0f * M_PI / sectorCount;
		const float stackStep = M_PI / stackCount;
		const float lengthInv = 1.0f / radius;
		uint32_t v = 0;

		for (uint32_t i = 0; i <= stackCount; ++i) {
			float stackAngle = M_PI / 2.0f - i * stackStep;
			float xy = radius * cosf(stackAngle);
			float z = radius * sinf(stackAngle);

			float vCoord = static_cast<float>(i) / stackCount;

			for (uint32_t j = 0; j <= sectorCount; ++j) {
				float sectorAngle = j * sectorStep;

				float x = xy * cosf(sectorAngle);
				float y = xy * sinf(sectorAngle);

				float uCoord = static_cast<float>(j) / sectorCount;

				Vertex& vert = vertices[v++];

				vert.Position = { x, y, z };
				vert.Normal = { x * lengthInv, y * lengthInv, z * lengthInv };
				vert.TexCoord = { uCoord, 1.0f - vCoord }; // flip V for OpenGL


				// Calculate tangent (along the longitude direction)
				// Tangent is perpendicular to the normal in the horizontal plane
				float tangentX = -sinf(sectorAngle);
				float tangentY = cosf(sectorAngle);
				float tangentZ = 0.0f;
				vert.Tangent = { tangentX, tangentY, tangentZ };

				// Calculate bitangent (along the latitude direction)
				// Bitangent = Normal × Tangent
				float bitangentX = -cosf(sectorAngle) * sinf(stackAngle);
				float bitangentY = -sinf(sectorAngle) * sinf(stackAngle);
				float bitangentZ = cosf(stackAngle);
				vert.Bitangent = { bitangentX, bitangentY, bitangentZ };
			}
		}

		uint32_t idx = 0;
		for (uint32_t i = 0; i < stackCount; ++i) {
			uint32_t k1 = i * (sectorCount + 1);
			uint32_t k2 = k1 + sectorCount + 1;

			for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
				if (i != 0) {
					indices[idx++] = k1;
					indices[idx++] = k2;
					indices[idx++] = k1 + 1;
				}

				if (i != (stackCount - 1)) {
					indices[idx++] = k1 + 1;
					indices[idx++] = k2;
					indices[idx++] = k2 + 1;
				}
			}
		}

	}
}
