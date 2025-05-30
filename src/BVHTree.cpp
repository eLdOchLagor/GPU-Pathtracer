#include "BVHTree.h"
#include "BoundingHelper.h"
#include "iostream"



BVHTree::BVHTree(std::vector<Primitive>& primitives) : primitives(primitives){
        // Initialize index buffer [0, 1, ..., N-1]
        triangleIndices.resize(primitives.size());
        std::iota(triangleIndices.begin(), triangleIndices.end(), 0); //Skapar en lista med v�rden fr�n 0 till triangleIndices.size().
        largestDepth = 0;
        smallestDepth = 1000;
        if (primitives.size() < 50) {
            maxPrimitives = 12;
        }
        else {
            maxPrimitives = 2;
        }
        buildRecursive(0, primitives.size(), 0); //startar byggandet av tr�det.
        //std::cout << this->nodes.size(); //Bara f�r debugging
		//traverseTree(); //Traversera tr�det f�r att se att det �r korrekt byggt.
}

//Ber�knar AABB f�r trianglarna fr�n och med start till start + count.
AABB BVHTree::computeBounds(int start, int count) {
    AABB bounds = computeAABB(primitives[triangleIndices[start]]);
    for (int i = start + 1; i < start + count; i++) {
        expandAABB(bounds, computeAABB(primitives[triangleIndices[i]]));
    }
    return bounds;
}

void BVHTree::rebuild(const std::vector<Primitive>& newPrims)
{
    primitives = newPrims;

    nodes.clear();
	triangleIndices.clear();
    largestDepth = 0;
    smallestDepth = 1000;
    triangleIndices.resize(primitives.size());
    std::iota(triangleIndices.begin(), triangleIndices.end(), 0);

    buildRecursive(0, primitives.size(), 0);
    std::cout << "Largest depth: " << largestDepth << "    Smallest depth: " << smallestDepth << "\n\n";
}

//Funktionen bygger tr�det uppifr�n, dvs den startar med roten och slutar med l�ven.
int BVHTree::buildRecursive(int start, int count, int depth) {
    
    //H�mta boundsen f�r de relevanta trianglarna (om vi �r i roten �r det alla, i l�ven �r det den enstaka triangeln).
    AABB bounds = computeBounds(start, count);
    //Skapa noden och allocata den till nodes.
    BVHNode node;
    node.bBoxMin = bounds.min;
    node.bBoxMax = bounds.max;

    int nodeIndex = nodes.size();
    nodes.push_back(node);

    //Om det �r en l�vnod s� skapar vi en korrekt l�vnod (inga children s� vi s�tter dem till -1).
    if (count <= maxPrimitives) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        nodes[nodeIndex].escapeIndex = nodeIndex + 1; // next node in pre-order
        if (depth < smallestDepth) {
            smallestDepth = depth;
        }
        if (depth > largestDepth) {
            largestDepth = depth;
        }
        return nodeIndex;
        
    }

    //Denh�r biten �r ett s�tt att bryta upp tr�det p�. Det finns b�ttre � denna bit av koden �r �verdrivet jobbig
    //pga att jag fr�gade chatten om en l�sning � dens l�sning  inneh�ll glm::vec3 och glm::max osv, vilket jag beh�vde jobba
    //runt d� den f�rs�kta anv�nda vec3[0] f�r att h�mta x elementet, vilket Ingemars vektorer inte har som en funktion.
    //TODO: Anv�nd en b�ttre heuristic.
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
    if (maxCentroid - minCentroid < 1e-3f) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        nodes[nodeIndex].escapeIndex = nodeIndex + 1; // next node in pre-order
        if (depth < smallestDepth) {
            smallestDepth = depth;
        }
        if (depth > largestDepth) {
            largestDepth = depth;
        }
        return nodeIndex;
    }
    float scale = NUM_BINS / (maxCentroid - minCentroid + 1e-5f); // Undvik div-by-zero
    

    for (int i = start; i < start + count; ++i) {
        const Primitive& p = primitives[triangleIndices[i]];
        vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
        int binIdx = std::min(NUM_BINS - 1, int((centroid[splitAxis] - minCentroid) * scale));
        bins[binIdx].bounds = mergeAABB(bins[binIdx].bounds, computeAABB(primitives[triangleIndices[i]]));
        bins[binIdx].count++;
    }

    AABB leftBounds[NUM_BINS - 1];
    int leftCounts[NUM_BINS - 1];
    AABB rightBounds[NUM_BINS - 1];
    int rightCounts[NUM_BINS - 1];

    // V�nster till H�ger
    AABB curBounds;
    int curCount = 0;
    for (int i = 0; i < NUM_BINS - 1; ++i) {
        curBounds = mergeAABB(curBounds, bins[i].bounds);
        curCount += bins[i].count;
        leftBounds[i] = curBounds;
        leftCounts[i] = curCount;
    }

    // H�ger to V�nster
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
    //float parentVolume = volume(bounds);

    for (int i = 0; i < NUM_BINS - 1; ++i) {
        float Ct = 0.5f;
        float Ci = 1.0f;

        float cost = Ct + (Ci * (
            leftCounts[i] * surfaceArea(leftBounds[i]) +
            rightCounts[i] * surfaceArea(rightBounds[i])
            )) / parentArea;
        float imbalance = std::abs(leftCounts[i] - rightCounts[i]) / float(leftCounts[i] + rightCounts[i]);

        if (cost < bestCost && imbalance < 0.9f) {
            bestCost = cost;
            bestSplit = i;
        }
    }
    int mid;
    if (bestCost > 1.0f) {
        // Fallback: median split
        int midIndex = start + count / 2;

        std::nth_element(
            triangleIndices.begin() + start,
            triangleIndices.begin() + midIndex,
            triangleIndices.begin() + start + count,
            [&](int a, int b) {
                vec3 centroidA = (primitives[a].vertex1 + primitives[a].vertex2 + primitives[a].vertex3) / 3.0f;
                vec3 centroidB = (primitives[b].vertex1 + primitives[b].vertex2 + primitives[b].vertex3) / 3.0f;
                return centroidA[splitAxis] < centroidB[splitAxis];
            }
        );

        mid = midIndex;
    }
    else {
        float binWidth = (maxCentroid - minCentroid) / float(NUM_BINS);
        float splitPos = minCentroid + binWidth * (bestSplit + 1);

        auto midIter = std::partition(
            triangleIndices.begin() + start,
            triangleIndices.begin() + start + count,
            [&](int idx) {
                const Primitive& p = primitives[idx];
                vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
                return centroid[splitAxis] < splitPos;
            }
        );
        mid = midIter - triangleIndices.begin();
    }

    //G�r recursion med children
    int leftChild = buildRecursive(start, mid - start, depth + 1);
    int rightChild = buildRecursive(mid, start + count - mid, depth + 1);

    //Initialisera noden. Om vi inte slapar ett l�v s� har det inte en specifik triangel,
    //s� trianglecount �r 0 och starttriangle finns ej s� vi s�tter den till -1
    nodes[nodeIndex].leftChild = leftChild;
    nodes[nodeIndex].rightChild = rightChild;
    nodes[nodeIndex].startTriangle = -1;
    nodes[nodeIndex].triangleCount = 0;

    // V�nster nods escapenod �r den h�gra noden i subtr�det.
    nodes[leftChild].escapeIndex = rightChild;


    nodes[nodeIndex].escapeIndex = nodes.size(); // Next in list

    // H�gra nodens escapenod �r denh�ra nodens escapenod.
    nodes[rightChild].escapeIndex = nodes[nodeIndex].escapeIndex;
    
    return nodeIndex;
}
//Crude test so that we can see that the tree is built correctly.
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