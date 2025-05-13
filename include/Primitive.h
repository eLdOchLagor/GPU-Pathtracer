#pragma once
#include "VectorUtils4.h"

struct Primitive {
    alignas(16) vec3 vertex1;
    alignas(16) vec3 vertex2;
    alignas(16) vec3 vertex3;
    alignas(16) vec3 color;
    alignas(16) vec3 normal;
    alignas(16) vec3 edge1;
    alignas(16) vec3 edge2;
    //alignas(16) vec3 edge3;
    alignas(4) int ID; // 0 == Triangle, 1 == Sphere
    alignas(4) float bounceOdds; //Odds that the ray would bounce off of the surface.
    alignas(4) int materialType; // 0 = diffuse, 1 = mirror, 2 = glass
    alignas(4) float ior; // Index of refraction
};