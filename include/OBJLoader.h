#pragma once

#include <string>
#include <vector>
#include <VectorUtils4.h>

class OBJLoader
{
public:
	// Prevent instantiation of the class
	OBJLoader() = delete;

	static bool loadOBJ(const std::string& path,
		std::vector<vec3>& out_vertices,
		std::vector<vec2>& out_uvs,
		std::vector<vec3>& out_normals);

private:

};
