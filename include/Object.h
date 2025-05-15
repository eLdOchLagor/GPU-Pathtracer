#pragma once

#include <glad/glad.h>
#include <vector>
#include <VectorUtils4.h>
#include "Primitive.h"
#include <string>
#include "OBJLoader.h"

class Object
{
public:
	Object() {};
	Object(std::string filePath);

    void CreateObjectFromModel(const std::string& path);

private:
	// For rendering using ray tracing
    std::vector<Primitive> primitives;

	// For rendering using rasterization
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals;
	// ----------------------------------

	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 modelMatrix;
};
