#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <vector>
#include "OBJLoader.h"
#include <VectorUtils4.h>
#include "Primitive.h"



struct Primitive {
    alignas(16) vec3 vertex1;
    alignas(16) vec3 vertex2;
    alignas(16) vec3 vertex3;
    alignas(16) vec3 color;
    alignas(16) vec3 normal;
    alignas(4) int ID; // 0 == Triangle, 1 == Sphere
    alignas(4) float bounceOdds; //Odds that the ray would bounce off of the surface.
    alignas(4) int materialType; // 0 = diffuse, 1 = mirror, 2 = glass
	alignas(4) float ior; // Index of refraction
};

class Scene
{
public:

    Scene(){};

    Scene(int presetID);

    void getRoom();
    void getSpheres();

    void CreateSceneFromModel(const std::string& path);
    

    std::vector<Primitive> primitives;
private:
	
};
