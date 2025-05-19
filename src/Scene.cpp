#include "Scene.h"

Scene::Scene(int presetID) {
    switch (presetID) {
    case 0:
        getRoom();
        break;
    case 1:
        getSpheres();
        break;
    case 2:
		CreateSceneFromModel("..\\models\\StanfordBunny348.obj", 0);
        break;
    case 3:
        CreateSceneFromModel("..\\models\\Bunny70K.obj", 0);
        break;
    case 4:
        getCrazyScene();
        break;
    default:
        break;
    }
}

//Riktigt tuff f�r datorn att hantera p� min sida.
void Scene::getCrazyScene() {
    getRoom();
    CreateSceneFromModel("..\\models\\StanfordBunny348.obj", primitives.size());
}

void Scene::CreateSceneFromModel(const std::string& path,int index = 0) {
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals;

	// Load the OBJ file
	OBJLoader::loadOBJ(path, vertices, uvs, normals);

    primitives.resize(vertices.size()/3 + index);
    areaLights.resize(1);
    pointLights.resize(0);

    size_t primIdx = index;
    for (size_t i = 0; i < vertices.size(); i+=3)
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
    //pointLights[0].position = vec3(5.5, 0.0, 6.0);
    //pointLights[0].radiance = vec3(1.0, 1.0, 1.0) / 25.f;

    areaLights[0].vertex1 = vec3(-2, 4.99, 8);
    areaLights[0].vertex2 = vec3(2, 4.99, 8);
    areaLights[0].vertex3 = vec3(2, 4.99, 11);
    areaLights[0].vertex4 = vec3(-2, 4.99, 11);
    areaLights[0].normal = vec3(0.0, -1.0, 0.0);
    areaLights[0].radiance = vec3(10.0, 10.0, 10.0);
}

void Scene::getSpheres() {
    primitives.resize(5);
    areaLights.resize(0);
    pointLights.resize(1);

    /*Primitive temp{vec3(-10.0f, -5.0f, 10.0f), vec3(-10.0f, -5.0f, -10.0f), vec3(10.0f, -5.0f, -10.0f), vec3(0.0f, 1.0f, 0.0f), vec3(79.0f, 163.0f, 146.0f) / 255.0f, 0, 1.0f, 'L'};
    primitives[1] = temp;

    Primitive temp2 = Primitive{ vec3(10.0, -5.0, -10.0), vec3(10.0, -5.0, 10.0), vec3(-10.0, -5.0, 10.0),  vec3(0.0f, 1.0f, 0.0f), vec3(79, 163, 146) / 255.0f, 0, 1.0f, 'L' };
    primitives.push_back(temp2);
    */
    //floor
        //triangle front

    pointLights[0].position = vec3(5.5, 0.0, 6.0);
    pointLights[0].radiance = vec3(1.0, 1.0, 1.0)/5.f;
    int i = 0;
    primitives[i].vertex1 = vec3(-10.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(-10.0f, -5.0f, -10.0f);
    primitives[i].vertex3 = vec3(10.0f, -5.0f, -10.0f);
    primitives[i].edge1 = primitives[i].vertex2 - primitives[i].vertex1;
    primitives[i].edge2 = primitives[i].vertex3 - primitives[i].vertex1;
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].smoothness = 0.0f;
    primitives[i].materialType = 0;
    primitives[i].ior = 1.0f;
    i++;
   
    //triangle mid right
    primitives[i].vertex1 = vec3(10.0f, -5.0f, -10.0f);
    primitives[i].vertex2 = vec3(10.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-10.0f, -5.0f, 10.0f);
    primitives[i].edge1 = primitives[i].vertex2 - primitives[i].vertex1;
    primitives[i].edge2 = primitives[i].vertex3 - primitives[i].vertex1;
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].smoothness = 0.0f;
    primitives[i].materialType = 0;
    primitives[i].ior = 1.0f;
    i++;

    //triangle mid right
    primitives[i].vertex1 = vec3(0.0f, -4.0f, 4.0f);
    primitives[i].vertex2 = vec3(1.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-10.0f, -5.0f, 10.0f);
    primitives[i].edge1 = primitives[i].vertex2 - primitives[i].vertex1;
    primitives[i].edge2 = primitives[i].vertex3 - primitives[i].vertex1;
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 1;
    primitives[i].smoothness = 0.0f;
    primitives[i].materialType = 0;
    primitives[i].ior = 1.0f;
    i++;

    //triangle mid right
    primitives[i].vertex1 = vec3(3.0f, -4.0f, 7.0f);
    primitives[i].vertex2 = vec3(0.75f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-10.0f, -5.0f, 10.0f);
    primitives[i].edge1 = primitives[i].vertex2 - primitives[i].vertex1;
    primitives[i].edge2 = primitives[i].vertex3 - primitives[i].vertex1;
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 1;
    primitives[i].smoothness = 0.0f;
    primitives[i].materialType = 0;
    primitives[i].ior = 1.0f;
    i++;
    //triangle mid right
    primitives[i].vertex1 = vec3(-6.0f, -2.0f, 10.0f);
    primitives[i].vertex2 = vec3(3.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-10.0f, -5.0f, 10.0f);
    primitives[i].edge1 = primitives[i].vertex2 - primitives[i].vertex1;
    primitives[i].edge2 = primitives[i].vertex3 - primitives[i].vertex1;
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 1;
    primitives[i].smoothness = 0.0f;
    primitives[i].materialType = 0;
    primitives[i].ior = 1.0f;
    i++;

    

}


void Scene::getRoom() {
    primitives.resize(25);

    vec3 e1 = vec3(0.0f);
    vec3 e2 = vec3(0.0f);
    int i = 0;

    // Room object
    Object room = Object("..\\models\\room.obj");
    objects.push_back(room);

    Object tempObj = {};

    //sphere 1
    Primitive temp = {};
    temp.vertex1 = vec3(0.0f, -3.5f, 8.0f);
    temp.vertex2 = vec3(1.5f, -5.0f, 10.0f);
    temp.vertex3 = vec3(-10.0f, -5.0f, 10.0f);
    temp.edge1 = temp.vertex2 - temp.vertex1;
    temp.edge2 = temp.vertex3 - temp.vertex1;
    temp.normal = vec3(0.0f, 1.0f, 0.0f);
    temp.color = vec3(255, 255, 255) / 255.0f;
    temp.ID = 1;
    temp.smoothness = 0.0f;
    temp.materialType = 2;
    temp.ior = 1.5f;

    tempObj.primitives.push_back(temp);
    objects.push_back(tempObj);
    tempObj = {};
    temp = {};

    //Sphere 2
    temp.vertex1 = vec3(-1.0f, -0.5f, 8.0f);
    temp.vertex2 = vec3(1.5f, -5.0f, 10.0f);
    temp.vertex3 = vec3(-10.0f, -5.0f, 10.0f);
    temp.edge1 = temp.vertex2 - temp.vertex1;
    temp.edge2 = temp.vertex3 - temp.vertex1;
    temp.normal = vec3(0.0f, 1.0f, 0.0f);
    temp.color = vec3(10, 10, 10);
    temp.ID = 1;
    temp.smoothness = 0.0f;
    temp.materialType = 3;
    temp.ior = 1.0f;

    tempObj.primitives.push_back(temp);
    objects.push_back(tempObj);
    tempObj = {};
    temp = {};

    //Light1 side1
    temp.vertex1 = vec3(-2, 4.95, 8);
    temp.vertex2 = vec3(2, 4.95, 8);
    temp.vertex3 = vec3(2, 4.95, 11);
    temp.edge1 = temp.vertex2 - temp.vertex1;
    temp.edge2 = temp.vertex3 - temp.vertex1;
    temp.normal = vec3(0.0f, -1.0f, 0.0f);
    temp.color = vec3(1.0, 1.0, 1.0);
    temp.ID = 0;
    temp.smoothness = 0.0f;
    temp.materialType = 3;
    temp.ior = 1.0f;

    tempObj.primitives.push_back(temp);
    objects.push_back(tempObj);
    tempObj = {};
    temp = {};

    //Light1 side2
    temp.vertex1 = vec3(2, 4.95, 11);
    temp.vertex2 = vec3(-2, 4.95, 11);
    temp.vertex3 = vec3(-2, 4.95, 8);
    temp.edge1 = temp.vertex2 - temp.vertex1;
    temp.edge2 = temp.vertex3 - temp.vertex1;
    temp.normal = vec3(0.0f, -1.0f, 0.0f);
    temp.color = vec3(1.0, 1.0, 1.0);
    temp.ID = 0;
    temp.smoothness = 0.0f;
    temp.materialType = 3;
    temp.ior = 1.0f;

    tempObj.primitives.push_back(temp);
    objects.push_back(tempObj);
}

