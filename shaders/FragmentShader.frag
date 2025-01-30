#version 460 core

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
    Sphere spheres[10];  
};

layout(std430, binding = 1) buffer TriangleBuffer {
    Triangle triangles[24];  
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





float triangleIntersectionTest(vec3 dir, Triangle targetTriangle) {

		vec3 d = normalize(dir);
		vec3 s = cameraPosition;

		// If negative, then the surface is visible for the ray
		
		if (dot(d, targetTriangle.triangleNormal) < 0.0)
		{
			
			vec3 c1 = targetTriangle.vertex2 - targetTriangle.vertex1;
			vec3 c2 = targetTriangle.vertex3 - targetTriangle.vertex1;

			vec3 P = cross(d, c2);
			float det = dot(c1, P);
			//if (det < 1e-8 && det > -1e-8) {
			//	return -1.0;
			//}
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
    float u = -(coord.x / screenWidth * imagePlaneWidth - imagePlaneWidth/2.0);
    float v = (coord.y / screenHeight - 1.0) * imagePlaneHeight + imagePlaneHeight/2.0;

    vec3 direction = normalize(forward + u * right + v * up);

	// Background color
	FragColor = vec4(0.2, 0.2, 0.2, 1);

	//Intersection tests for spheres, then triangles.
	for(int i = 0; i < spheres.length(); i++){
		float hitSphere = sphereIntersectionTest(direction, spheres[i]);
		if (hitSphere > 0.0) {
		// Normal calculations for sphere
			vec3 hitPoint = cameraPosition + hitSphere * direction;
			vec3 normal = normalize(hitPoint - spheres[i].spherePOS);
			FragColor = vec4(-normal.z, 0, 0, 1);
		}
	}
	for(int q = 0; q < triangles.length(); q++){
		float hitTriangle = triangleIntersectionTest(direction, triangles[q]);
		if(hitTriangle > 0.0){
			vec3 normal = triangles[q].triangleNormal;
			FragColor = vec4(triangles[q].triangleColor,1);
		}
	}

}

