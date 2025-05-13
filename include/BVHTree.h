#pragma once
#include "VectorUtils4.h"
#include "Primitive.h"
#include "AABB.h"
#include "BoundingHelper.h"
#include <vector>
#include <numeric> // For std::iota

struct BVHNode {
	vec3 bBoxMin;
	int leftChild;
	vec3 bBoxMax;
	int rightChild;
	int startTriangle;
	int triangleCount;

	int pad0;
	int pad1;
};



class BVHTree {
public:
	BVHTree(const std::vector<Primitive>& primitives);
	const std::vector<BVHNode>& getNodes() { return nodes; }
	const std::vector<int>& getIndices() { return triangleIndices; }
private:
	const std::vector<Primitive>& primitives;
	std::vector<int> triangleIndices;
	std::vector<BVHNode> nodes;
	AABB computeBounds(int start, int count);
	int buildRecursive(int start, int count);
};