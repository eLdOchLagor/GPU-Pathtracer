#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include <VectorUtils4.h>

class Application
{
public:
	Application(int width, int height, const std::string& title);
	~Application();
	
	void Run();

	//void SetScene(Scene* scene);
	//Scene* GetScene() const;

	// Callback functions have to be static
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void processInput(GLFWwindow* window);

	GLFWwindow* window;

private:
	int screenWidth, screenHeight;
	
	GLuint textures[2];
	Camera mainCamera;
	Scene roomScene;
	GLuint framebuffer;

	unsigned int PathtraceShader;
	unsigned int DisplayShader;

	int frameCount = 0;
	float previousTime = 0;
	float deltaTime = 0;
	int currentTexture;
	unsigned int VAO;

	void Init();
	GLFWwindow* createWindow(const std::string& title);
	static void Application::clearAccumulationBuffer(GLFWwindow* window);
};
