#pragma once


#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <VectorUtils4.h>
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"

class Application
{
public:
	Application(int width, int height, const std::string& title);
	~Application();
	
	void Run();

	//void SetScene(Scene* scene);
	//Scene* GetScene() const;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

private:
	int screenWidth, screenHeight;
	GLFWwindow* window;
	GLuint textures[2];
	Camera mainCamera;
	Scene roomScene;

	void Init();
	GLFWwindow* createWindow(const std::string& title);
};
