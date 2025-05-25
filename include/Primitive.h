#pragma once
#include "VectorUtils4.h"

struct Primitive {
    vec3 vertex1;
    int ID; // 0 == Triangle, 1 == Sphere
    vec3 vertex2;
    float smoothness; //Odds that the ray would bounce off of the surface.
    vec3 vertex3;
    int materialType; // 0 = diffuse, 1 = mirror, 2 = glass
    vec3 color;
    float ior; // Index of refraction
    alignas(16) vec3 normal;
    alignas(16) vec3 edge1;
    alignas(16) vec3 edge2;
   
    
    
    
};