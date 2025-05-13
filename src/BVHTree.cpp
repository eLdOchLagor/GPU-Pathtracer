#include "BVHTree.h"
#include "BoundingHelper.h"
#include "iostream"



BVHTree::BVHTree(const std::vector<Primitive>& primitives) : primitives(primitives){
        // Initialize index buffer [0, 1, ..., N-1]
        triangleIndices.resize(primitives.size());
        std::iota(triangleIndices.begin(), triangleIndices.end(), 0);

        buildRecursive(0, primitives.size());
        std::cout << this->nodes.size();
}
	
AABB BVHTree::computeBounds(int start, int count) {
    AABB bounds = computeAABB(primitives[triangleIndices[start]]);
    for (int i = start + 1; i < start + count; i++) {
        expandAABB(bounds, computeAABB(primitives[triangleIndices[i]]));
    }
    return bounds;
}


//Funktionen bygger tr�det uppifr�n, dvs den startar med roten och slutar med l�ven.
int BVHTree::buildRecursive(int start, int count) {
    
    //H�mta boundsen f�r de relevanta trianglarna (om vi �r i roten �r det alla, i l�ven �r det den enstaka triangeln).
    AABB bounds = computeBounds(start, count);
    //Skapa noden och allocata den till nodes.
    BVHNode node;
    node.bBoxMin = bounds.min;
    node.bBoxMax = bounds.max;

    int nodeIndex = nodes.size();
    nodes.push_back(node);

    //Om det �r en l�vnod s� skapar vi en korrekt l�vnod (inga children s� vi s�tter dem till -1).
    if (count <= 2) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        return nodeIndex;
    }

    //Denh�r biten �r ett s�tt att bryta upp tr�det p�. Det finns b�ttre � denna bit av koden �r �verdrivet jobbig
    //pga att jag fr�gade chatten om en l�sning � dens l�sning  inneh�ll glm::vec3 och glm::max osv, vilket jag beh�vde jobba
    //runt d� den f�rs�kta anv�nda vec3[0] f�r att h�mta x elementet, vilket Ingemars vektorer inte har som en funktion.
    //TODO: Anv�nd en b�ttre heuristic.
    int splitAxis = longestAxis(bounds);
    float centroidSum = 0;
    for (int i = start; i < start + count; i++) {
        const Primitive& p = primitives[triangleIndices[i]];
        const vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
        if (splitAxis == 0) { centroidSum += centroid.x; }
        else if (splitAxis == 1) { centroidSum += centroid.y; }
        else if (splitAxis == 2) { centroidSum += centroid.z; }
    }
    float centroidMid = centroidSum / count;

    auto midIter = std::partition(
        triangleIndices.begin() + start,
        triangleIndices.begin() + start + count,
        [&](int idx) {
            const Primitive& p = primitives[idx];
            vec3 centroid = (p.vertex1 + p.vertex2 + p.vertex3) / 3.0f;
            if (splitAxis == 0) { return centroid.x < centroidMid; }
            else if (splitAxis == 1) { return centroid.y < centroidMid; }
            else if (splitAxis == 2) { return centroid.z < centroidMid; }
            
        }
    );

    int mid = midIter - triangleIndices.begin();

    //Edge case f�r en l�vnod, om vi inte g�r detta kan vi hamna i en inf loop.
    if (mid == start || mid == start + count) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        return nodeIndex;
    }

    //G�r recursion med children
    int leftChild = buildRecursive(start, mid - start);
    int rightChild = buildRecursive(mid, start + count - mid);

    //Initialisera noden. Om vi inte slapar ett l�v s� har det inte en specifik triangel,
    //s� trianglecount �r 0 och starttriangle finns ej s� vi s�tter den till -1
    nodes[nodeIndex].leftChild = leftChild;
    nodes[nodeIndex].rightChild = rightChild;
    nodes[nodeIndex].startTriangle = -1;
    nodes[nodeIndex].triangleCount = 0;
    
    return nodeIndex;
}