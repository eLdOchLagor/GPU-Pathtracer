#version 460 core

#define M_PI 3.1415926535897932384626433832795
struct Primitive{
	vec3 vertex1;
	vec3 vertex2;
	vec3 vertex3;
	vec3 color;
	vec3 normal;
	int ID; // 0 == Triangle, 1 == Sphere
	float bounceOdds; //Odds that the ray would bounce off of the surface.
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

layout(std430, binding = 0) buffer SphereBuffer {
    Sphere spheres[1];  
};

layout(std430, binding = 1) buffer TriangleBuffer {
    Triangle triangles[29];  
};

layout(std430, binding = 2) buffer PrimitiveBuffer{
	Primitive primitives[100];
};

out vec4 FragColor;

uniform float time;

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
			closestDistance = i;
		}
	}
	return vec2(closestDistance, closestIndex);
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

	float scalar_radiance = (cosx * cosy) / (length(di) * length(di));
	float A = length(e1) * length(e2);

	radiance = vec3(AreaLight.radiance * scalar_radiance * A/M_PI) * surfaceColor;

	return radiance;
}


void main() {
    vec4 coord = gl_FragCoord;

	seed = uint(coord.y * screenWidth + coord.x);

    // Coordinates in imagePlane
    float u = -(coord.x / screenWidth * imagePlaneWidth - imagePlaneWidth/2.0);
    float v = (coord.y / screenHeight - 1.0) * imagePlaneHeight + imagePlaneHeight/2.0;

    vec3 direction = normalize(forward + u * right + v * up);

	// Background color
	FragColor = vec4(0.2, 0.2, 0.2, 1);

	//Intersection tests for spheres, then triangles.
	float closestTimeSphere = -1.0;
	float closestTimeTriangle = -1.0;
	Sphere closestSphere;
	Triangle closestTriangle;
	
	/*for(int i = 0; i < spheres.length(); i++){
		float distance_1 = sphereIntersectionTest(direction, spheres[i]);
		if ((distance_1 < closestTimeSphere || closestTimeSphere < 0.0) && distance_1 > 0.0) {
			closestTimeSphere = distance_1;
			closestSphere = spheres[i];
			vec3 t = cameraPosition + distance_1 * direction;
			vec3 normal = normalize(t - spheres[i].spherePOS);

			vec3 directIllumination = calculateDirectIllumination(direction, t, normal, vec3(1.0,0.0,0.0));
			FragColor = vec4(directIllumination,1.0);
		}
	}
	for(int q = 0; q < triangles.length(); q++){
		float distance_2 = triangleIntersectionTest(direction, triangles[q]);
		if((distance_2 < closestTimeTriangle || closestTimeTriangle < 0.0 ) && distance_2 > 0.0 && (closestTimeSphere > distance_2 || closestTimeSphere < 0.0)){
			closestTimeTriangle = distance_2;
			closestTriangle = triangles[q];
			vec3 normal = triangles[q].triangleNormal;
			vec3 t = cameraPosition + distance_2 * direction;
			
			vec3 directIllumination = calculateDirectIllumination(direction, t, normal, triangles[q].triangleColor);
			FragColor = vec4(directIllumination,1.0);		
		}
	}*/
	Ray ray = Ray(direction, cameraPosition, vec3(0));
	
	
	vec2 hit = intersectionTest(ray);
	FragColor = vec4(primitives[int(hit.y)].color,1.0);

}

