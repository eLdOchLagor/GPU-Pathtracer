#include "Object.h"

Object::Object(std::string filePath)
{
	CreateObjectFromModel(filePath);
}

void Object::CreateObjectFromModel(const std::string& path)
{
	// Load the OBJ file into member variables for rasterization
	OBJLoader::loadOBJ(path, vertices, uvs, normals);

	// Convert loaded data into primitives for ray tracing
	primitives.resize(vertices.size() / 3);
	size_t primIdx = 0;
	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		primitives[primIdx].vertex1 = vertices[i];
		primitives[primIdx].vertex2 = vertices[i + 1];
		primitives[primIdx].vertex3 = vertices[i + 2];
		primitives[primIdx].edge1 = primitives[primIdx].vertex2 - primitives[primIdx].vertex1;
		primitives[primIdx].edge2 = primitives[primIdx].vertex3 - primitives[primIdx].vertex1;
		primitives[primIdx].normal = normals[i];
		primitives[primIdx].color = vec3(255, 100, 100) / 255.0f; // Arbitrary color
		primitives[primIdx].ID = 0;
		primitives[primIdx].smoothness = 0.0f;
		primitives[primIdx].materialType = 0;
		primitives[primIdx].ior = 1.0f;

		primIdx++;
	}
}

void Object::BindBuffers()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertex positions
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Normals
	glGenBuffers(1, &NBO); // NBO = Normal Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Object::RenderObject()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // If it does not work, make sure it is Uint
}