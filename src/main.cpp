#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MAIN
#include "VectorUtils4.h"
#include "Application.h"

int main() {
    
	Application app(800*2, 600*2, "TSBK07 - Raytracer");
	app.Run();

    return 0;
}