#pragma once

struct Mesh {
	std::vector<float> vertices;
	std::vector<uint32_t> indices;

	Mesh() = default;
	Mesh(std::vector<float> v, std::vector<uint32_t> i)
		: vertices(std::move(v)), indices(std::move(i)) {}
};