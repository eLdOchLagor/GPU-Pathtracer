#pragma once

#include "Primitive.h"
#include "AABB.h"
#include "VectorUtils4.h"
#include <algorithm>

//Om vi hade glm skulle dessa funktioner vara typ 3 rader långa :)
inline AABB computeAABB(const Primitive& tri) {
	AABB aabb;
	float minx, miny, minz, maxx, maxy, maxz;

	minx = fmin(fmin(tri.vertex1.x, tri.vertex2.x), tri.vertex3.x);
	miny = fmin(fmin(tri.vertex1.y, tri.vertex2.y), tri.vertex3.y);
	minz = fmin(fmin(tri.vertex1.z, tri.vertex2.z), tri.vertex3.z);

	maxx = fmax(fmax(tri.vertex1.x, tri.vertex2.x), tri.vertex3.x);
	maxy = fmax(fmax(tri.vertex1.y, tri.vertex2.y), tri.vertex3.y);
	maxz = fmax(fmax(tri.vertex1.z, tri.vertex2.z), tri.vertex3.z);

	aabb.min = vec3(minx, miny, minz);
	aabb.max = vec3(maxx, maxy, maxz);

	return aabb;
}

inline void expandAABB(AABB& base, const AABB& other) {
	float minx, miny, minz, maxx, maxy, maxz;

	minx = fmin(base.min.x, other.min.x);
	miny = fmin(base.min.y, other.min.y);
	minz = fmin(base.min.z, other.min.z);

	maxx = fmin(base.max.x, other.max.x);
	maxy = fmin(base.max.y, other.max.y);
	maxz = fmin(base.max.z, other.max.z);

	base.min = vec3(minx, miny, minz);
	base.max = vec3(maxx, maxy, maxz);
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