#version 330 core

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

vec3 sphereCenter = vec3(0.0, 0.0, 1.0);
float sphereRadius = 0.5;

float sphereIntersectionTest(vec3 dir) {
		float c1 = dot(dir, dir);
		float c2 = 2.0 * dot(dir, cameraPosition - sphereCenter);
		float c3 = dot(cameraPosition - sphereCenter, cameraPosition - sphereCenter) - sphereRadius * sphereRadius;

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
    float v = (1.0 - coord.y / screenHeight) * imagePlaneHeight - imagePlaneHeight/2.0;

    vec3 direction = normalize(forward + u * right + v * up);

    float hit = sphereIntersectionTest(direction);

	// Background color
	FragColor = vec4(0, 0, 0, 1);

	if (hit > 0.0) {
		FragColor = vec4(1, 0, 0, 1);
	}

}

