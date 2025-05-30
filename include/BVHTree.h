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
	int escapeIndex;
	
	int pad1;
};



class BVHTree {
public:
	BVHTree(std::vector<Primitive>& primitives);

	void rebuild(const std::vector<Primitive>& newPrims);

	const std::vector<BVHNode>& getNodes() { return nodes; }
	const std::vector<int>& getIndices() { return triangleIndices; }
private:
	int largestDepth;
	int smallestDepth;
	int maxPrimitives;
	std::vector<Primitive> primitives; //Trianglarna
	std::vector<int> triangleIndices; //Denna listan hittar trianglarna relaterat till noderna i tr�det.
	std::vector<BVHNode> nodes; //Noderna med children � AABB
	AABB computeBounds(int start, int count);
	int buildRecursive(int start, int count, int depth);
	void traverseTree();
};