#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <vector>;
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class Scene
{
public:
	Scene() {
		vertices = std::vector<glm::fvec3>();
		spheres = std::vector<glm::fvec4>();

	}

	void AddSphere(glm::fvec3 pos = glm::fvec3(0), float radius = 1.0f) {
		spheres.push_back(glm::fvec4(pos, radius));
	}

	void AddTriangle(glm::fvec3 vert1 = glm::fvec3(-0.5f,-0.5f,1.0f), glm::fvec3 vert2 = glm::fvec3(0.5f, -0.5f, 1.0f), glm::fvec3 vert3 = glm::fvec3(0.0f, 0.5f, 1.0f)) {
		vertices.push_back(vert1);
		vertices.push_back(vert2);
		vertices.push_back(vert3);
		glm::fvec3 c1 = vert3 - vert2;
		glm::fvec3 c2 = vert2 - vert1;
		glm::fvec3 normal = glm::normalize(glm::cross(c1, c2));
		std::cout << normal.z;
		vertices.push_back(normal);
	}


private:
	std::vector<glm::fvec3> vertices;
	std::vector<glm::fvec4> spheres;
};
