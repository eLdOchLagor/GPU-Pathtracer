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


//Funktionen bygger trädet uppifrån, dvs den startar med roten och slutar med löven.
int BVHTree::buildRecursive(int start, int count) {
    
    //Hämta boundsen för de relevanta trianglarna (om vi är i roten är det alla, i löven är det den enstaka triangeln).
    AABB bounds = computeBounds(start, count);
    //Skapa noden och allocata den till nodes.
    BVHNode node;
    node.bBoxMin = bounds.min;
    node.bBoxMax = bounds.max;

    int nodeIndex = nodes.size();
    nodes.push_back(node);

    //Om det är en lövnod så skapar vi en korrekt lövnod (inga children så vi sätter dem till -1).
    if (count <= 2) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        return nodeIndex;
    }

    //Denhär biten är ett sätt att bryta upp trädet på. Det finns bättre å denna bit av koden är överdrivet jobbig
    //pga att jag frågade chatten om en lösning å dens lösning  innehöll glm::vec3 och glm::max osv, vilket jag behövde jobba
    //runt då den försökta använda vec3[0] för att hämta x elementet, vilket Ingemars vektorer inte har som en funktion.
    //TODO: Använd en bättre heuristic.
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

    //Edge case för en lövnod, om vi inte gör detta kan vi hamna i en inf loop.
    if (mid == start || mid == start + count) {
        nodes[nodeIndex].startTriangle = start;
        nodes[nodeIndex].triangleCount = count;
        nodes[nodeIndex].leftChild = -1;
        nodes[nodeIndex].rightChild = -1;
        return nodeIndex;
    }

    //Gör recursion med children
    int leftChild = buildRecursive(start, mid - start);
    int rightChild = buildRecursive(mid, start + count - mid);

    //Initialisera noden. Om vi inte slapar ett löv så har det inte en specifik triangel,
    //så trianglecount är 0 och starttriangle finns ej så vi sätter den till -1
    nodes[nodeIndex].leftChild = leftChild;
    nodes[nodeIndex].rightChild = rightChild;
    nodes[nodeIndex].startTriangle = -1;
    nodes[nodeIndex].triangleCount = 0;
    
    return nodeIndex;
}