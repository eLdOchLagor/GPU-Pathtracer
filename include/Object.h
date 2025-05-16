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
	void UpdateModelMatrix();

	void BindBuffers();
	void RenderObject();

	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
	vec3 scale = vec3(1.0f);

	mat4 modelMatrix = S(scale.x, scale.y, scale.z) * T(position.x, position.y, position.z) * Rz(rotation.z * M_PI / 180.0f) * Ry(rotation.y * M_PI / 180.0f) * Rx(rotation.x * M_PI / 180.0f);

	unsigned int VAO, VBO, NBO;

	// For rendering using rasterization
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals;
	// ----------------------------------

	// For rendering using ray tracing
	std::vector<Primitive> primitives;

private:

	std::string name = "New Object";
};
