#version 460 core

#define M_PI 3.1415926535897932384626433832795

#define GLOSSY 0
#define MIRROR 1
#define TRANSMISSIVE 2
#define LIGHT 3

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

struct HitResult {
    float t;
    int index;
};

struct PointLight {
	vec3 position;
	vec3 radiance;
};

struct AreaLight
{
	vec3 vertex1;
	vec3 vertex2;
	vec3 vertex3;
	vec3 vertex4;
	vec3 normal;
	vec3 radiance;
};

struct Ray{
	vec3 direction;
	vec3 startPoint;
	vec3 endPoint;
};

struct Sphere{
	vec3 spherePOS;
	float sphereRadius;
};

struct Triangle{
	vec3 vertex1;
	vec3 vertex2;
	vec3 vertex3;
	vec3 triangleNormal;
	vec3 triangleColor;
};

struct Primitive{
	vec3 vertex1;
	vec3 vertex2;
	vec3 vertex3;
	vec3 color;
	vec3 normal;
	vec3 edge1;
    vec3 edge2;
	int ID; // 0 == Triangle, 1 == Sphere
	float smoothness; //Odds that the ray would bounce off of the surface.
	int materialType;
	float ior;
};

layout(std430, binding = 0) buffer PrimitiveBuffer{
	Primitive primitives[];
};

layout(std430, binding = 1) buffer BVHNodeBuffer{
	BVHNode nodes[];
};

layout(std430, binding = 2) buffer TriangleIndices {
    int triangleIndices[];
};

layout(std430, binding = 3) buffer PointLights{
	PointLight pointLights[];
};

layout(std430, binding = 4) buffer AreaLights{
	AreaLight areaLights[];
};

out vec4 FragColor;

uniform sampler2D accumTexture;
uniform int frameCount;

uniform vec3 cameraPosition;
uniform vec3 forward;
uniform vec3 right;
uniform vec3 up;
uniform float imagePlaneWidth;
uniform float imagePlaneHeight;

uniform int numberOfSamples;
uniform int maxBounces;

uniform int screenWidth;
uniform int screenHeight;

uniform int NUM_OF_POINT_LIGHTS;
uniform int NUM_OF_AREA_LIGHTS;



in vec3 pos;

// Temporarily hardcoded light --------------------------------------------------------------------
struct Light
{
	vec3 vertex1;
	vec3 vertex2;
	vec3 vertex3;
	vec3 vertex4;
	vec3 normal;
	vec3 radiance;
};

Light mainLight = Light(vec3(-2, 4.99, 8), vec3(2, 4.99, 8), vec3(2, 4.99, 11), vec3(-2, 4.99, 11), vec3(0.0, -1.0, 0.0), vec3(10.0, 10.0, 10.0));

// --------------------------------------------------------------------------------------------------

// Random number generator --------------------------------------------------------------------------
uint seed;

// PCG Hash Taken from https://www.shadertoy.com/view/ctj3Wc
// Märker nu att man kan se ett repeterande m�nster, kanske ska kolla alternativ, l�s kommentarer i l�nken ovan f�r mer info
uint PCGHash()
{
    seed = seed * 747796405u + 2891336453u;
    uint state = seed;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float RandomFloat(uint inputSeed){
	return (PCGHash() / float(0xFFFFFFFFU));
}
// ----------------------------------------------------------------------------------------------------
HitResult traverseBVHTree(Ray ray, vec3 rayDirInv);

float triangleIntersectionTest(Ray currentRay, Primitive targetTriangle) {

		vec3 d = normalize(currentRay.direction);
		vec3 s = currentRay.startPoint;

		// If negative, then the surface is visible for the ray
		
		if (dot(d, targetTriangle.normal) < 0.0)
		{
			
			vec3 c1 = targetTriangle.edge1;
			vec3 c2 = targetTriangle.edge2;

			vec3 P = cross(d, c2);
			float det = dot(c1, P);
			
			vec3 T = s - targetTriangle.vertex1;
			float u = dot(T, P) / det;
			if (u < 0 || u > 1) {
				return -1.0;
			}

			vec3 Q = cross(T, c1);
			float v = dot(d, Q) / det;
			if (v < 0 || v > 1-u) {
				return -1.0;
			}
			float t = dot(c2, Q) / det;
			if (t < 0) {
				return -1.0;
			}
			return t;
			

		}
		return -1.0;
	}

float sphereIntersectionTest(Ray currentRay, Primitive targetSphere) {
		float c1 = dot(currentRay.direction,currentRay.direction);
		float c2 = 2.0 * dot(currentRay.direction, currentRay.startPoint - targetSphere.vertex1);
		float c3 = dot(currentRay.startPoint - targetSphere.vertex1, currentRay.startPoint - targetSphere.vertex1) - targetSphere.vertex2.x * targetSphere.vertex2.x;

		float arg = c2 * c2 - 4.0 * c1 * c3;

		if (arg >= 0.0)
		{
			float t1 = (-c2 + sqrt(arg)) / (2.0 * c1);
			float t2 = (-c2 - sqrt(arg)) / (2.0 * c1);
			
			float t = -1.0;
			if (t1 > 0 && t2 > 0) {
				t = min(t1, t2);
			}
			else if (t1 > 0) {
				t = t1;
			}
	
			else if (t2 > 0) {
				t = t2;
			}

			return t;
		}
		
		return -1.0;
}

bool isInShadow(vec3 startPoint, vec3 y){
	Ray shadowRay = Ray(normalize(y-startPoint), startPoint, vec3(0.0));
	float distance = length(y - startPoint);
	vec3 rayDirectionInv = 1.0/shadowRay.direction;
	HitResult hit = traverseBVHTree(shadowRay, rayDirectionInv);
	if(hit.t > 0.0 && hit.t < distance){
		return true;
	}
	return false;
}

//Returns the closest distance hit. First value of returned vec2 is the distance, second value is the index of the object int the primitives array.
vec2 intersectionTest(Ray currentRay){
	float closestDistance = -1.0;
	int closestIndex = -1;
	for(int i = 0; i < primitives.length(); i++){
		float t = -1.0;
		if(primitives[i].ID == 0){
			t = triangleIntersectionTest(currentRay , primitives[i]);
		}
		else if(primitives[i].ID == 1){
			 t = sphereIntersectionTest(currentRay, primitives[i]);
		}
		if((t < closestDistance || closestDistance < 0.0) && t > 0.0){
			closestDistance = t;
			closestIndex = i;
		}
	}

	return vec2(closestDistance, closestIndex);
}

bool intersectAABB(vec3 rayOrigin, vec3 rayDirInv, vec3 minB, vec3 maxB) {
    vec3 tMin = (minB - rayOrigin) * rayDirInv;
    vec3 tMax = (maxB - rayOrigin) * rayDirInv;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
	
    return tFar >= max(tNear, 0.0);
}

float intersectAABBOther(vec3 rayOrigin, vec3 rayDirInv, vec3 minB, vec3 maxB) {
    vec3 tMin = (minB - rayOrigin) * rayDirInv;
    vec3 tMax = (maxB - rayOrigin) * rayDirInv;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
	bool didHit = tFar > tNear && tFar > 0;
	return didHit ? tNear : 100000;
    
}


HitResult traverseBVHTree(Ray ray, vec3 rayDirInv) {
    int nodeIndex = 0;
    float closestT = 100000;
    int closestPrimIdx = -1;

    

    while (nodeIndex != -1) {
       
        if (nodeIndex < 0 || nodeIndex >= nodes.length()) {
			if(closestPrimIdx != -1){return HitResult(closestT,closestPrimIdx);}
            return HitResult(-1,-1);
        }
		
        BVHNode node = nodes[nodeIndex];

        if (intersectAABBOther(ray.startPoint, rayDirInv, node.bBoxMin, node.bBoxMax) < closestT) {
            if (node.triangleCount > 0) {
                // Leaf node
                for (int i = 0; i < node.triangleCount; ++i) {
                    int primIndex = triangleIndices[node.startTriangle + i];
                    Primitive prim = primitives[primIndex];

                    float t = (prim.ID == 0) ? triangleIntersectionTest(ray, prim) : sphereIntersectionTest(ray, prim);

                    if (t > 0.0f && (t < closestT)) {
                        closestT = t;
                        closestPrimIdx = primIndex;
                    }
                }
                nodeIndex = node.escapeIndex; // ➜ Go to next node
            } else {
                nodeIndex = node.leftChild; // ➜ Go to left child
            }
        } else {
            nodeIndex = node.escapeIndex; // ➜ Skip subtree
        }
    }

    return HitResult(closestT,closestPrimIdx);
}



float fresnelSchlick(float cosTheta, float ior) {
	float r0 = pow((1.0 - ior) / (1.0 + ior), 2.0);
	return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calculateDirectIllumination(vec3 dir, vec3 hitPoint, vec3 normal, vec3 surfaceColor, AreaLight light){
	vec3 radiance = vec3(0.0, 0.0, 0.0);
	
	
	for(int i = 0; i < NUM_OF_AREA_LIGHTS; i++ ){
		vec3 e1 = areaLights[i].vertex2 - areaLights[i].vertex1;
		vec3 e2 = areaLights[i].vertex4 - areaLights[i].vertex1;
	
		float s = RandomFloat(seed);
		float t = RandomFloat(seed);

		vec3 y = areaLights[i].vertex1 + s * e1 + t * e2;
		vec3 di = y - hitPoint;

		

		if (!isInShadow(hitPoint + 0.0001*normal, y)){
			float cosx = dot(normal, normalize(di));
			float cosy = dot(-areaLights[i].normal, normalize(di));

			// Make sure that surfaces facing away from the lightsource dont give negative values, these values give wrong result
			cosx = max(0.0, cosx);
			cosy = max(0.0, cosy);
			float scalar_radiance = (cosx * cosy) / (length(di) * length(di));
			float A = length(e1) * length(e2);

			radiance += vec3(areaLights[i].radiance * scalar_radiance * A/M_PI) * surfaceColor;
		}

	}
	for(int i = 0; i < NUM_OF_POINT_LIGHTS; i++ ){
		if(!isInShadow(hitPoint, pointLights[i].position)){
			radiance += pointLights[i].radiance;
		}
	}
	

	return radiance;
}


Ray diffuseReflection(Ray r, Primitive hitSurface, float randAz, float randInc) {
	float x = cos(randAz) * sin(randInc);
	float y = sin(randAz) * sin(randInc);
	float z = cos(randInc);
	vec3 tangent; 
	vec3 bitangent;
	vec3 normal = hitSurface.normal;

	tangent = normalize(-r.direction + dot(normal, r.direction) * normal);
	bitangent = normalize(cross(normal, tangent));

	vec3 worldDir = normalize(normal * z + tangent * x + bitangent * y);

	vec3 startPoint = r.endPoint;
	Ray newRay = Ray(worldDir, startPoint, vec3(0.0));

	return newRay;
}

Ray generateCameraRay(ivec2 pixelCoord){
	float jitterX = RandomFloat(seed) - 0.5;
	float jitterY = RandomFloat(seed) - 0.5;

	float u = -((pixelCoord.x + jitterX) / float(screenWidth) * imagePlaneWidth - imagePlaneWidth / 2.0);
	float v = ((pixelCoord.y + jitterY) / float(screenHeight) - 1.0) * imagePlaneHeight + imagePlaneHeight / 2.0;

	vec3 direction = normalize(forward + u * right + v * up);
	return Ray(direction, cameraPosition, vec3(0.0));
}

vec3 raytrace(Ray ray) {
	vec3 accumulatedColor = vec3(0.0);
	vec3 importance = vec3(1.0); // Keeps track of ray contribution
	
	for (int i = 0; i < maxBounces; i++) {
		vec3 rayDirInv = 1.0 / ray.direction;
		HitResult hit = traverseBVHTree(ray, rayDirInv);
		
		//vec2 hit = intersectionTest(ray);

		if (hit.index == -1) {
			accumulatedColor += importance * vec3(0.2); // Background
			break;
		}

		Primitive hitSurface = primitives[hit.index];

		ray.endPoint = ray.startPoint + hit.t * ray.direction;

		// Compute surface normal
		vec3 normal = (hitSurface.ID == 1)
			? normalize(ray.startPoint - hitSurface.vertex1) // Sphere normal
			: hitSurface.normal;
		
		// Mirror surface
		if (hitSurface.materialType == MIRROR) {
			ray.direction = normalize(reflect(ray.direction, normal));
			ray.startPoint = ray.endPoint;
			i--;
			continue;
		}

		// Diffuse surface
		if (hitSurface.materialType == GLOSSY) {
			vec3 directIllumination = calculateDirectIllumination(
				ray.direction, ray.endPoint, normal, hitSurface.color
			, areaLights[0]);

			accumulatedColor += importance * directIllumination;
			importance *= hitSurface.color;

			float randChoice = RandomFloat(seed);

			if (randChoice < hitSurface.smoothness) {
				// Reflect
				ray.direction = reflect(ray.direction, normal);
				
			} else {
				// Diffuse reflection
				float randomValue1 = RandomFloat(seed);
				float randomValue2 = RandomFloat(seed);

				float randInclination = acos(sqrt(1.0 - randomValue1));
				float randAzimuth = 2.0 * M_PI * randomValue2;
				ray = diffuseReflection(ray, hitSurface, randAzimuth, randInclination);
			}

			ray.startPoint = ray.endPoint + 0.001 * ray.direction;
		}

		// Transmissive surface
		if (hitSurface.materialType == TRANSMISSIVE) {
			float ior = hitSurface.ior;

			// Calculate normal if sphere
			vec3 hitPoint = ray.endPoint;
			vec3 normal = (hitSurface.ID == 1) ? normalize(hitPoint - hitSurface.vertex1) : hitSurface.normal;

			
			float eta = 1.0 / ior;
			bool entering = dot(ray.direction, normal) < 0.0;
	
			if (!entering) {
				normal = -normal;
				eta = ior;
			}

			float cosTheta = clamp(dot(-ray.direction, normal), 0.0, 1.0);

			float fresnel = fresnelSchlick(cosTheta, ior);

			if (RandomFloat(seed) < fresnel) {
				// Reflect
				ray.direction = reflect(ray.direction, normal);
			}
			else{
				// Refract
				vec3 refracted = refract(ray.direction, normal, eta);
				if (length(refracted) == 0.0){
					
					// Total internal reflection
					ray.direction = reflect(ray.direction, normal);
				}
				else {
					ray.direction = refracted;
				}

			}

			ray.startPoint = ray.endPoint + 0.001 * ray.direction;
			importance *= hitSurface.color;
		}
		if(hitSurface.materialType == LIGHT){
			accumulatedColor += importance * hitSurface.color;
			break;
		}
	}

	return accumulatedColor;
}

void main() {
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
	seed = uint(pixelCoord.y * screenWidth + pixelCoord.x) ^ (frameCount * 1664525u);
	vec2 texCoords = gl_FragCoord.xy / vec2(textureSize(accumTexture, 0));
	vec3 prevColor = texture(accumTexture, texCoords).rgb;

	vec3 accumulatedColor = vec3(0.0);

	for(int i = 0; i < numberOfSamples; i++)
	{
		Ray ray = generateCameraRay(pixelCoord);
		accumulatedColor += raytrace(ray);
	}
	
	accumulatedColor /= numberOfSamples;

	// Output final color
	vec3 ac = (prevColor * float(frameCount) + accumulatedColor) / float(frameCount + 1); // Blend with previous color
    FragColor = vec4(ac, 1.0);
	
}

