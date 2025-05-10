#include "Application.h"

#define MAIN
#include "VectorUtils4.h"

//float lastX = screenWidth/2.0f, lastY = screenHeight/2.0f;
float pitch, yaw;

int frameCount = 0;
float previousTime = 0;
float deltaTime = 0;

int main() {
    
	Application app(800, 600, "TSBK07 - Raytracer");
	app.Run();

    return 0;
}