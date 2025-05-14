#pragma once
#include "VectorUtils4.h"

struct PointLight {
	alignas(16) vec3 position;
	alignas(16) vec3 radiance;
};

struct AreaLight
{
	alignas(16) vec3 vertex1;
	alignas(16) vec3 vertex2;
	alignas(16) vec3 vertex3;
	alignas(16) vec3 vertex4;
	alignas(16) vec3 normal;
	alignas(16) vec3 radiance;
};