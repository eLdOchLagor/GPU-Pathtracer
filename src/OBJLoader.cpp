
#include <iostream>
#include <fstream>
#include <sstream>
#include "OBJLoader.h"

bool OBJLoader::loadOBJ(const std::string& path, std::vector<vec3>& out_vertices, std::vector<vec2>& out_uvs, std::vector<vec3>& out_normals)
{
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<vec3> temp_vertices;
    std::vector<vec2> temp_uvs;
    std::vector<vec3> temp_normals;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (type == "vt") {
            vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (type == "vn") {
            vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (type == "f") {
            unsigned int vi[3], uvi[3], ni[3];
            char slash;

			// TODO: Loader only works if there is vertex, uv, and normal data. Need to be handled differntly if no UV data is present.
            for (int i = 0; i < 3; ++i) {
                ss >> vi[i] >> slash >> uvi[i] >> slash >> ni[i];
                vertexIndices.push_back(vi[i]);
                uvIndices.push_back(uvi[i]);
                normalIndices.push_back(ni[i]);
            }
        }
    }

	// Use the indices to correctly construct the final vertex, uv, and normal vectors
    for (size_t i = 0; i < vertexIndices.size(); i++) {
        out_vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
		//out_uvs.push_back(temp_uvs[uvIndices[i] - 1]); // Commented out to avoid UVs
        out_normals.push_back(temp_normals[normalIndices[i] - 1]);
    }

    file.close();
    return true;
}

