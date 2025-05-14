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
#include "Light.h"
class Scene
{
public:

    Scene(){};

    Scene(int presetID);

    void getRoom();
    void getSpheres();
    void CreateSceneFromModel(const std::string& path);
    

    std::vector<Primitive> primitives;
    std::vector<PointLight> pointLights;
    std::vector<AreaLight> areaLights;
private:
	
};
