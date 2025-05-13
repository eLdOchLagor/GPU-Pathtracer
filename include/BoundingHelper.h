#pragma once

#include "Primitive.h"
#include "AABB.h"
#include "VectorUtils4.h"
#include <algorithm>


inline AABB computeAABB(const Primitive& tri) {
	AABB aabb;
	vec3 max = vec3::max(vec3::max(tri.vertex1, tri.vertex2), tri.vertex3);
	vec3 min = vec3::min(vec3::min(tri.vertex1, tri.vertex2), tri.vertex3);

	aabb.min = min;
	aabb.max = max;

	return aabb;
}

inline void expandAABB(AABB& base, const AABB& other) {
	float minx, miny, minz, maxx, maxy, maxz;

	vec3 max = vec3::max(base.max, other.max);
	vec3 min = vec3::min(base.min, other.min);

	base.min = min;
	base.max = max;
}

inline vec3 centerOfAABB(const AABB& aabb) {
	return 0.5f * (aabb.min + aabb.max);
}

inline int longestAxis(const AABB& aabb) {
	vec3 extent = aabb.max - aabb.min;
	
	//TODO: kolla om abs behövs
	if (extent.x > extent.y && extent.x > extent.z) return 0;
	if (extent.y > extent.z) return 1;
	return 2;
}