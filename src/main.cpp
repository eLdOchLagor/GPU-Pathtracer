#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"
#include "Application.h"

#define MAIN

#include "VectorUtils4.h"




// GLFW Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);
void clearAccumulationBuffer();

int screenWidth = 800;
int screenHeight = 600;

float lastX = screenWidth/2.0f, lastY = screenHeight/2.0f;
float pitch, yaw;

int frameCount = 0;
float previousTime = 0;
float deltaTime = 0;

float verts[] = {
    //bottom left Triangle
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    //top right Triangle
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};

Camera mainCamera;
GLuint framebuffer;

// Texture for storing accumulated color
GLuint textures[2];

int main() {
    
	Application app(screenWidth, screenHeight, "TSBK07 - Raytracer");
	app.Run();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    bool cameraMoved = false;
    float cameraSpeed = 6.0f * deltaTime;

    // NOTE: Left and right are reversed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mainCamera.position += cameraSpeed * mainCamera.GetForward();
        cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mainCamera.position -= cameraSpeed * mainCamera.GetForward();
        cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mainCamera.position += mainCamera.GetRight() * cameraSpeed;
        cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mainCamera.position -= mainCamera.GetRight() * cameraSpeed;
        cameraMoved = true;
    }
        
    if (cameraMoved)
    {
        frameCount = 0;
        clearAccumulationBuffer();
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset != 0.0)
    {
        float currentFOV = mainCamera.GetFOV();
        mainCamera.SetFOV(currentFOV - yoffset);

        // Redo these checks, it still resets accumulation currently, make sure it doesnt
        if (mainCamera.GetFOV() > 180.0f)
        {
            mainCamera.SetFOV(179.0f);
            return;
        }
        if (mainCamera.GetFOV() < 0.0f)
        {
            mainCamera.SetFOV(1.0f);
            return;
        }

        frameCount = 0;
        clearAccumulationBuffer();

    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    vec3 forwardDirection = vec3(0.0f);
    // As with wasd movement, left and right are opposite
    forwardDirection.z = cos(yaw * M_PI/180.0f) * cos(pitch * M_PI / 180.0f);
    forwardDirection.y = sin(pitch * M_PI / 180.0f);
    forwardDirection.x = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);

    mainCamera.SetForward(forwardDirection);

    // Reset accumulation
    frameCount = 0;
    clearAccumulationBuffer();
}

void clearAccumulationBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Bind both textures and clear them
    for (int i = 0; i < 2; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Reset to black
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}