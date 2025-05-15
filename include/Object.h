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
	void SetName(const std::string& name) { this->name = name; }
	std::string GetName() { return name; }

	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
	vec3 scale = vec3(0.0f);

private:
	// For rendering using ray tracing
    std::vector<Primitive> primitives;

	// For rendering using rasterization
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals;
	// ----------------------------------

	std::string name = "New Object";

	

	mat4 modelMatrix;
};
