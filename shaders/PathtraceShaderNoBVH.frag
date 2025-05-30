#version 460 core

#define M_PI 3.1415926535897932384626433832795

#define DIFFUSE 0
#define MIRROR 1
#define TRANSMISSIVE 2
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

int maxBounces = 5;
int samples = 5;

out vec4 FragColor;

uniform sampler2D accumTexture;
uniform int frameCount;

uniform vec3 cameraPosition;
uniform vec3 forward;
uniform vec3 right;
uniform vec3 up;
uniform float imagePlaneWidth;
uniform float imagePlaneHeight;

uniform int screenWidth;
uniform int screenHeight;


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

Light AreaLight = Light(vec3(-2, 4.99, 8), vec3(2, 4.99, 8), vec3(2, 4.99, 11), vec3(-2, 4.99, 11), vec3(0.0, -1.0, 0.0), vec3(10.0, 10.0, 10.0));
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

float triangleIntersectionTest(Ray currentRay, Primitive targetTriangle) {

		vec3 d = normalize(currentRay.direction);
		vec3 s = currentRay.startPoint;

		// If negative, then the surface is visible for the ray
		
		if (dot(d, targetTriangle.normal) < 0.0)
		{
			
			vec3 c1 = targetTriangle.vertex2 - targetTriangle.vertex1;
			vec3 c2 = targetTriangle.vertex3 - targetTriangle.vertex1;

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

	for(int i = 0; i < primitives.length(); i++){
		
		if (primitives[i].ID == 1 &&  primitives[i].materialType == 2){ // Skip if it hits a transmissive sphere (Special case)
			continue;
		}
		
		float t = -1.0;

		if(primitives[i].ID == 0){
			t = triangleIntersectionTest(shadowRay, primitives[i]);
		}
		else if(primitives[i].ID == 1){
			t = sphereIntersectionTest(shadowRay, primitives[i]);
		}
		
		if(t > 0.0 && t < distance) return true;
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

float fresnelSchlick(float cosTheta, float ior) {
	float r0 = pow((1.0 - ior) / (1.0 + ior), 2.0);
	return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calculateDirectIllumination(vec3 dir, vec3 hitPoint, vec3 normal, vec3 surfaceColor){
	vec3 radiance = vec3(0.0, 0.0, 0.0);

	vec3 e1 = AreaLight.vertex2 - AreaLight.vertex1;
	vec3 e2 = AreaLight.vertex4 - AreaLight.vertex1;

	float s = RandomFloat(seed);
	float t = RandomFloat(seed);

	vec3 y = AreaLight.vertex1 + s * e1 + t * e2;
	vec3 di = y - hitPoint;

	float cosx = dot(normal, normalize(di));
	float cosy = dot(-AreaLight.normal, normalize(di));

	// Make sure that surfaces facing away from the lightsource dont give negative values, these values give wrong result
	cosx = max(0.0, cosx);
	cosy = max(0.0, cosy);

	if (!isInShadow(hitPoint + 0.0001*normal, y)){
		float scalar_radiance = (cosx * cosy) / (length(di) * length(di));
		float A = length(e1) * length(e2);

		radiance = vec3(AreaLight.radiance * scalar_radiance * A/M_PI) * surfaceColor;
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
		vec2 hit = intersectionTest(ray);

		if (hit.x < 0.0) {
			accumulatedColor += importance * vec3(0.2, 0.2, 0.2); // Background color
			break;
		}

		ray.endPoint = ray.startPoint + hit.x * ray.direction;
		Primitive hitSurface = primitives[int(hit.y)];

		// Compute surface normal
		vec3 normal = (hitSurface.ID == 1)
			? normalize(ray.startPoint - hitSurface.vertex1) // Sphere normal
			: hitSurface.normal;

		// Mirror surface
		if (hitSurface.materialType == MIRROR) {
			ray.direction = normalize(reflect(ray.direction, normal));
			ray.startPoint = ray.endPoint;
			continue;
		}

		// Diffuse surface
		if (hitSurface.materialType == DIFFUSE) {
			vec3 directIllumination = calculateDirectIllumination(
				ray.direction, ray.endPoint, normal, hitSurface.color
			);

			accumulatedColor += importance * directIllumination;
			importance *= hitSurface.color;

			// Russian roulette
			float randomValue1 = RandomFloat(seed);
			float randomValue2 = RandomFloat(seed);

			float randInclination = acos(sqrt(1.0 - randomValue1));
			float randAzimuth = 2.0 * M_PI * randomValue2;
			float rr = randAzimuth / hitSurface.smoothness;

			if (rr <= 2.0 * M_PI && i != maxBounces - 1) {
				ray = diffuseReflection(ray, hitSurface, randAzimuth, randInclination);
			} else {
				break;
			}
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
	}

	return accumulatedColor;
}

void main() {
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
	seed = uint(pixelCoord.y * screenWidth + pixelCoord.x) ^ (frameCount * 1664525u);
	vec2 texCoords = gl_FragCoord.xy / vec2(textureSize(accumTexture, 0));
	vec3 prevColor = texture(accumTexture, texCoords).rgb;

	vec3 accumulatedColor = vec3(0.0);

	for(int i = 0; i < samples; i++)
	{
		Ray ray = generateCameraRay(pixelCoord);
		accumulatedColor += raytrace(ray);
	}
	
	accumulatedColor /= samples;

	// Output final color
	vec3 ac = (prevColor * float(frameCount) + accumulatedColor) / float(frameCount + 1); // Blend with previous color
    FragColor = vec4(ac, 1.0);
	
}
