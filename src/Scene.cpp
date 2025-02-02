#include "Scene.h"

void Scene::getRoom() {
    // Initializes 24 empty Primitives in the std::vector
    primitives.resize(24);

    vec3 e1 = vec3(0.0f);
    vec3 e2 = vec3(0.0f);
    int i = 0;
    //floor
        //triangle front

    primitives[i].vertex1 = vec3(0.0f, -5.0f, -3.0f);
    primitives[i].vertex2 = vec3(6.0f, -5.0f, 0.0f);
    primitives[i].vertex3 = vec3(-6.0f, -5.0f, 0.0f);
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //triangle mid right
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
    primitives[i].vertex2 = vec3(6.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //triangle mid left
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
    primitives[i].vertex2 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-6.0f, -5.0f, 0.0f);
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //triangle back
    primitives[i].vertex1 = vec3(0.0f, -5.0f, 13.0f);
    primitives[i].vertex2 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(6.0f, -5.0f, 10.0f);
    primitives[i].normal = vec3(0.0f, 1.0f, 0.0f);
    primitives[i].color = vec3(79, 163, 146) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //backroom walls
        //leftwall back
    primitives[i].vertex1 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(0.0f, -5.0f, 13.0f);
    primitives[i].vertex3 = vec3(0.0f, 5.0f, 13.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(224, 204, 177) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 0.0f;
    primitives[i].padding = 'L';
    i++;
    //leftwall front
    primitives[i].vertex1 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(0.0f, 5.0f, 13.0f);
    primitives[i].vertex3 = vec3(-6.0f, 5.0f, 10.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(224, 204, 177) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 0.0f;
    primitives[i].padding = 'L';
    i++;
    //backroom walls
        //leftwall back
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(0.0f, 5.0f, 13.0f);
    primitives[i].vertex3 = vec3(0.0f, -5.0f, 13.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(224, 204, 177) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //leftwall front
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(6.0f, 5.0f, 10.0f);
    primitives[i].vertex3 = vec3(0.0f, 5.0f, 13.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(224, 204, 177) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //right walls 
        //bottom wall
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
    primitives[i].vertex2 = vec3(6.0f, 5.0f, 0.0f);
    primitives[i].vertex3 = vec3(6.0f, -5.0f, 10.0f);
    primitives[i].normal = vec3(-1.0f, 0.0f, 0.0f);
    primitives[i].color = vec3(86, 77, 120) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //top wall
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(6.0f, 5.0f, 0.0f);
    primitives[i].vertex3 = vec3(6.0f, 5.0f, 10.0f);
    primitives[i].normal = vec3(-1.0f, 0.0f, 0.0f);
    primitives[i].color = vec3(86, 77, 120) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;

    //left walls 
        //bottom wall
    primitives[i].vertex1 = vec3(-6.0f, -5.0f, 0.0f);
    primitives[i].vertex2 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-6.0f, 5.0f, 0.0f);
    primitives[i].normal = vec3(1.0f, 0.0f, 0.0f);
    primitives[i].color = vec3(15, 173, 207) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //top wall
    primitives[i].vertex1 = vec3(-6.0f, -5.0f, 10.0f);
    primitives[i].vertex2 = vec3(-6.0f, 5.0f, 10.0f);
    primitives[i].vertex3 = vec3(-6.0f, 5.0f, 0.0f);
    primitives[i].normal = vec3(1.0f, 0.0f, 0.0f);
    primitives[i].color = vec3(15, 173, 207) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //frontroom walls
        //left bottom
    primitives[i].vertex1 = vec3(0.0f, -5.0f, -3.0f);
    primitives[i].vertex2 = vec3(-6.0f, -5.0f, 0.0f);
    primitives[i].vertex3 = vec3(0.0f, 5.0f, -3.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(255, 99, 131) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //left top
    primitives[i].vertex1 = vec3(-6.0f, -5.0f, 0.0f);
    primitives[i].vertex2 = vec3(-6.0f, 5.0f, 0.0f);
    primitives[i].vertex3 = vec3(0.0f, 5.0f, -3.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(255, 99, 131) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //frontroom walls
        //right bottom
    primitives[i].vertex1 = vec3(0.0f, -5.0f, -3.0f);
    primitives[i].vertex2 = vec3(0.0f, 5.0f, -3.0f);
    primitives[i].vertex3 = vec3(6.0f, -5.0f, 0.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(255, 99, 131) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 0.0f;
    primitives[i].padding = 'L';
    i++;
    //right top
    primitives[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
    primitives[i].vertex2 = vec3(0.0f, 5.0f, -3.0f);
    primitives[i].vertex3 = vec3(6.0f, 5.0f, 0.0f);
    e1 = primitives[i].vertex2 - primitives[i].vertex1;
    e2 = primitives[i].vertex3 - primitives[i].vertex2;
    primitives[i].normal = vec3(normalize(cross(e2, e1)));
    primitives[i].color = vec3(255, 99, 131) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 0.0f;
    primitives[i].padding = 'L';
    i++;
    //roof
        //triangle front
    primitives[i].vertex1 = vec3(0.0f, 5.0f, -3.0f);
    primitives[i].vertex2 = vec3(-6.0f, 5.0f, 0.0f);
    primitives[i].vertex3 = vec3(6.0f, 5.0f, 0.0f);
    primitives[i].normal = vec3(0.0f, -1.0f, 0.0f);
    primitives[i].color = vec3(243, 186, 42) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //triangle mid right
    primitives[i].vertex1 = vec3(6.0f, 5.0f, 0.0f);
    primitives[i].vertex2 = vec3(-6.0f, 5.0f, 10.0f);
    primitives[i].vertex3 = vec3(6.0f, 5.0f, 10.0f);
    primitives[i].normal = vec3(0.0f, -1.0f, 0.0f);
    primitives[i].color = vec3(243, 186, 42) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //triangle mid left
    primitives[i].vertex1 = vec3(6.0f, 5.0f, 0.0f);
    primitives[i].vertex2 = vec3(-6.0f, 5.0f, 0.0f);
    primitives[i].vertex3 = vec3(-6.0f, 5.0f, 10.0f);
    primitives[i].normal = vec3(0.0f, -1.0f, 0.0f);
    primitives[i].color = vec3(243, 186, 42) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';
    i++;
    //triangle back
    primitives[i].vertex1 = vec3(-6.0f, 5.0f, 10.0f);
    primitives[i].vertex2 = vec3(0.0f, 5.0f, 13.0f);
    primitives[i].vertex3 = vec3(6.0f, 5.0f, 10.0f);
    primitives[i].normal = vec3(0.0f, -1.0f, 0.0f);
    primitives[i].color = vec3(243, 186, 42) / 255.0f;
    primitives[i].ID = 0;
    primitives[i].bounceOdds = 1.0f;
    primitives[i].padding = 'L';


}
