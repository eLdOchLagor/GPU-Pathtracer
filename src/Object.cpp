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
	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		primitives[i / 3].vertex1 = vertices[i];
		primitives[i / 3].vertex2 = vertices[i + 1];
		primitives[i / 3].vertex3 = vertices[i + 2];
		primitives[i / 3].edge1 = primitives[i / 3].vertex2 - primitives[i / 3].vertex1;
		primitives[i / 3].edge2 = primitives[i / 3].vertex3 - primitives[i / 3].vertex1;
		primitives[i / 3].normal = normals[i];
		primitives[i / 3].color = vec3(255, 100, 100) / 255.0f; // Arbitrary color
		primitives[i / 3].ID = 0;
		primitives[i / 3].smoothness = 0.0f;
		primitives[i / 3].materialType = 0;
		primitives[i / 3].ior = 1.0f;
	}
}