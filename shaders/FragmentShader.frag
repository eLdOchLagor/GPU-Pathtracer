#version 460 core

struct Sphere{
	vec3 spherePOS;
	float sphereRadius;
};

layout(std430, binding = 0) buffer SphereBuffer {
    Sphere spheres[10];  // Limited by UBO size (adjust as needed)
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

vec3 sphereCenter = vec3(0.0, 1.0, 1.0);
float sphereRadius = 0.5;

vec3 vertex1 = vec3(-0.5,-0.5,0.5);
vec3 vertex2 = vec3(0.5,-0.5,0.5);
vec3 vertex3 = vec3(0.0,0.5,1.0);
vec3 normal = vec3(0.0,0.0,-1.0);


float triangleIntersectionTest(vec3 dir) {

		vec3 d = normalize(dir);
		vec3 s = cameraPosition;

		// If negative, then the surface is visible for the ray
		
		if (dot(d, normal) < 0.0)
		{

			vec3 c1 = vertex2 - vertex1;
			vec3 c2 = vertex3 - vertex1;

			vec3 P = cross(d, c2);
			float det = dot(c1, P);
			//if (det < 1e-8 && det > -1e-8) {
			//	return -1.0;
			//}
			vec3 T = s - vertex1;
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

float sphereIntersectionTest(vec3 dir, Sphere targetSphere) {
		float c1 = dot(dir, dir);
		float c2 = 2.0 * dot(dir, cameraPosition - targetSphere.spherePOS);
		float c3 = dot(cameraPosition - targetSphere.spherePOS, cameraPosition - targetSphere.spherePOS) - targetSphere.sphereRadius * targetSphere.sphereRadius;

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

void main() {
   
    vec4 coord = gl_FragCoord;

    // Coordinates in imagePlane
    float u = coord.x / screenWidth * imagePlaneWidth - imagePlaneWidth/2.0;
    float v = (coord.y / screenHeight - 1.0) * imagePlaneHeight + imagePlaneHeight/2.0;

    vec3 direction = normalize(forward + u * right + v * up);

    //float hit = sphereIntersectionTest(direction);

	

	// Background color
	FragColor = vec4(0.2, 0.2, 0.2, 1);

	
	for(int i = 0; i < spheres.length(); i++){
		float hit = sphereIntersectionTest(direction, spheres[i]);
		if (hit > 0.0) {
		// Normal calculations for sphere
			vec3 hitPoint = cameraPosition + hit * direction;
			vec3 normal = normalize(hitPoint - sphereCenter);
			FragColor = vec4(-normal.z, 0, 0, 1);
		}
	}

}

