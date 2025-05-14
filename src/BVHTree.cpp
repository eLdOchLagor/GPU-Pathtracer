#include "BVHTree.h"
#include "BoundingHelper.h"
#include "iostream"



BVHTree::BVHTree(const std::vector<Primitive>& primitives) : primitives(primitives){
        // Initialize index buffer [0, 1, ..., N-1]
        triangleIndices.resize(primitives.size());
        std::iota(triangleIndices.begin(), triangleIndices.end(), 0); //Skapar en lista med värden från 0 till triangleIndices.size().

        buildRecursive(0, primitives.size(), 0); //startar byggandet av trädet.
        //std::cout << this->nodes.size(); //Bara för debugging
		//traverseTree(); //Traversera trädet för att se att det är korrekt byggt.
}

//Beräknar AABB för trianglarna från och med start till start + count.
AABB BVHTree::computeBounds(int start, int count) {
    AABB bounds = computeAABB(primitives[triangleIndices[start]]);
    for (int i = start + 1; i < start + count; i++) {
        expandAABB(bounds, computeAABB(primitives[triangleIndices[i]]));
    }
    return bounds;
}


//Funktionen bygger trädet uppifrån, dvs den startar med roten och slutar med löven.
int BVHTree::buildRecursive(int start, int count, int depth) {
    
    //Hämta boundsen för de relevanta trianglarna (om vi är i roten är det alla, i löven är det den enstaka triangeln).
    AABB bounds = computeBounds(start, count);
    //Skapa noden och allocata den till nodes.
    BVHNode node;
    node.bBoxMin = bounds.min;
    node.bBoxMax = bounds.max;

    int nodeIndex = nodes.size();
    nodes.push_back(node);

    //Om det är en lövnod så skapar vi en korrekt lövnod (inga children så vi sätter dem till -1).
    if (count <= 6) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        nodes[nodeIndex].escapeIndex = nodeIndex + 1; // next node in pre-order
        return nodeIndex;
        
    }

    //Denhär biten är ett sätt att bryta upp trädet på. Det finns bättre å denna bit av koden är överdrivet jobbig
    //pga att jag frågade chatten om en lösning å dens lösning  innehöll glm::vec3 och glm::max osv, vilket jag behövde jobba
    //runt då den försökta använda vec3[0] för att hämta x elementet, vilket Ingemars vektorer inte har som en funktion.
    //TODO: Använd en bättre heuristic.
    AABB centroidBounds;
    for (int i = start; i < start + count; ++i) {
        const Primitive& p = primitives[triangleIndices[i]];
        vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
        AABB pointBounds;
        pointBounds.min = centroid;
        pointBounds.max = centroid;
        expandAABB(centroidBounds, pointBounds);
    }
    int splitAxis = longestAxis(centroidBounds);

    constexpr int NUM_BINS = 16;
    struct Bin {
        AABB bounds;
        int count = 0;
    };

    Bin bins[NUM_BINS];

    float minCentroid = centroidBounds.min[splitAxis];
    float maxCentroid = centroidBounds.max[splitAxis];
    float scale = NUM_BINS / (maxCentroid - minCentroid + 1e-5f); // Avoid div-by-zero

    for (int i = start; i < start + count; ++i) {
        const Primitive& p = primitives[triangleIndices[i]];
        vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
        int binIdx = std::min(NUM_BINS - 1, int((centroid[splitAxis] - minCentroid) * scale));
        bins[binIdx].bounds = mergeAABB(bins[binIdx].bounds, computeBounds(triangleIndices[i], 1));
        bins[binIdx].count++;

    }

    AABB leftBounds[NUM_BINS - 1];
    int leftCounts[NUM_BINS - 1];
    AABB rightBounds[NUM_BINS - 1];
    int rightCounts[NUM_BINS - 1];

    // Left to right
    AABB curBounds;
    int curCount = 0;
    for (int i = 0; i < NUM_BINS - 1; ++i) {
        curBounds = mergeAABB(curBounds, bins[i].bounds);
        curCount += bins[i].count;
        leftBounds[i] = curBounds;
        leftCounts[i] = curCount;
    }

    // Right to left
    curBounds = {};
    curCount = 0;
    for (int i = NUM_BINS - 1; i > 0; --i) {
        curBounds = mergeAABB(curBounds, bins[i].bounds);
        curCount += bins[i].count;
        rightBounds[i - 1] = curBounds;
        rightCounts[i - 1] = curCount;
    }

    float bestCost = std::numeric_limits<float>::max();
    int bestSplit = -1;
    float parentArea = surfaceArea(bounds);

    for (int i = 0; i < NUM_BINS - 1; ++i) {
        float cost = 1.0f + (
            leftCounts[i] * surfaceArea(leftBounds[i]) +
            rightCounts[i] * surfaceArea(rightBounds[i])
            ) / parentArea;

        if (cost < bestCost) {
            bestCost = cost;
            bestSplit = i;
        }
    }

    float splitPos = minCentroid + (bestSplit + 1) / float(NUM_BINS) * (maxCentroid - minCentroid);

    auto midIter = std::partition(
        triangleIndices.begin() + start,
        triangleIndices.begin() + start + count,
        [&](int idx) {
            const Primitive& p = primitives[idx];
            vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
            return centroid[splitAxis] < splitPos;
        }
    );

    int mid = midIter - triangleIndices.begin();

    if (mid == start || mid == start + count) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        nodes[nodeIndex].escapeIndex = nodeIndex + 1; // next node in pre-order
        return nodeIndex;
    }


    //Gör recursion med children
    int leftChild = buildRecursive(start, mid - start, depth + 1);
    int rightChild = buildRecursive(mid, start + count - mid, depth + 1);

    //Initialisera noden. Om vi inte slapar ett löv så har det inte en specifik triangel,
    //så trianglecount är 0 och starttriangle finns ej så vi sätter den till -1
    nodes[nodeIndex].leftChild = leftChild;
    nodes[nodeIndex].rightChild = rightChild;
    nodes[nodeIndex].startTriangle = -1;
    nodes[nodeIndex].triangleCount = 0;

    // After building left subtree, set its escape to point to right subtree
    nodes[leftChild].escapeIndex = rightChild;


    nodes[nodeIndex].escapeIndex = nodes.size(); // Next in list

    // Right child's escape is this node's escape (assigned by parent)
    nodes[rightChild].escapeIndex = nodes[nodeIndex].escapeIndex;

    
    
    return nodeIndex;
}

void BVHTree::traverseTree() {
	// Traverse the BVH tree and perform operations on each node
	for (const auto& node : nodes) {
		// Perform operations on the node
		std::cout << "Node AABB: " << node.bBoxMin.x << ", " << node.bBoxMin.y << ", " << node.bBoxMin.z << " to "
			<< node.bBoxMax.x << ", " << node.bBoxMax.y << ", " << node.bBoxMax.z << std::endl;
        if (node.rightChild == -1 && node.leftChild == -1) {
            for (int i = node.startTriangle; i < node.startTriangle + node.triangleCount; i++) {
				const Primitive& temp = primitives[triangleIndices[i]];
                std::cout << "Triangle " << triangleIndices[i] << " with vertex1:" << temp.vertex1.x << ", " << temp.vertex1.y << ", " << temp.vertex1.z << ", vertex2: "
                    <<temp.vertex2.x << ", " << temp.vertex2.y << ", " << temp.vertex2.z << ", vertex3: " 
                   <<temp.vertex3.x << ", " << temp.vertex3.y << ", " << temp.vertex3.z << std::endl;
        } }
	}
}