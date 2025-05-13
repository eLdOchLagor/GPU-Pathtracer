#pragma once
#include "VectorUtils4.h"
#include <cfloat>

struct AABB {
	vec3 min = vec3(FLT_MAX);
	vec3 max = vec3(-FLT_MAX);
};