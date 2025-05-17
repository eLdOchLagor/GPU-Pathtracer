#pragma once

#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "BVHTree.h"

class Application
{
public:
	Application(int width, int height, const std::string& title);
	~Application();
	
	void Run();

	void RenderPathtraced();
	void RenderRasterized();

	void BindBuffersPathtraced();
	void BindBuffersRasterized();

	//void SetScene(Scene* scene);
	//Scene* GetScene() const;

	// Callback functions have to be static
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void processInput(GLFWwindow* window);

	GLFWwindow* window;
	bool isRastered = true;

private:
	int screenWidth, screenHeight;
	
	GLuint textures[2];
	Camera mainCamera;
	Scene currentScene;
	GLuint framebuffer;

	unsigned int PathtraceShader;
	unsigned int DisplayShader;
	unsigned int RasterShader;

	bool presetSceneButton1 = false;
	bool presetSceneButton2 = false;

	int selectedIndex = -1;
	int frameCount = 0;
	float previousTime = 0;
	float deltaTime = 0;
	int currentTexture;
	unsigned int VAO;
	BVHTree bvhTree;

	int numberOfSamples = 1;
	int maxBounces = 5;

	void Init();
	GLFWwindow* createWindow(const std::string& title);
	void RenderGui(GLFWwindow* window);
	static void Application::clearAccumulationBuffer(GLFWwindow* window);
};
