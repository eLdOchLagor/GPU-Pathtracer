#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

int screenWidth = 800;
int screenHeight = 600;

Camera mainCamera = Camera(glm::fvec3(0.0f, 0.0f, -1.0f), glm::fvec3(0.0f, 0.0f, 1.0f), glm::fvec3(0.0f, 1.0f, 0.0f), 80.0f, screenWidth, screenHeight);

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "TSBK07 - Raytracer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader initialization ----------------------------------------------------------------
    Shader VertexShader = Shader("..\\shaders\\VertexShader.vert", GL_VERTEX_SHADER);
    Shader FragmentShader = Shader("..\\shaders\\FragmentShader.frag", GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, VertexShader.shaderID);
    glAttachShader(shaderProgram, FragmentShader.shaderID);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as it is no longer needed
    glDeleteShader(VertexShader.shaderID);
    glDeleteShader(FragmentShader.shaderID);

    // ----------------------------------------------------------------------------------------

    unsigned int timeLoc = glGetUniformLocation(shaderProgram, "time");

    // Camera Uniforms
    unsigned int positionLoc = glGetUniformLocation(shaderProgram, "cameraPosition");
    unsigned int forwardLoc = glGetUniformLocation(shaderProgram, "forward");
    unsigned int rightLoc = glGetUniformLocation(shaderProgram, "right");
    unsigned int upLoc = glGetUniformLocation(shaderProgram, "up");

    unsigned int imagePlaneWidthLoc = glGetUniformLocation(shaderProgram, "imagePlaneWidth");
    unsigned int imagePlaneHeightLoc = glGetUniformLocation(shaderProgram, "imagePlaneHeight");

    unsigned int screenWidthLoc = glGetUniformLocation(shaderProgram, "screenWidth");
    unsigned int screenHeightLoc = glGetUniformLocation(shaderProgram, "screenHeight");

    // JONATANS EXTRA FINA TESTKOD
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glViewport(0, 0, screenWidth, screenHeight);

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();

        // Pass Uniforms
        glUniform1f(timeLoc, currentFrame);

        // Camera Uniforms
        glUniform3fv(positionLoc, 1, &mainCamera.GetPosition()[0]);
        glUniform3fv(forwardLoc, 1, &mainCamera.GetForward()[0]);
        glUniform3fv(rightLoc, 1, &mainCamera.GetRight()[0]);
        glUniform3fv(upLoc, 1, &mainCamera.GetUp()[0]);
        glUniform1f(imagePlaneHeightLoc, mainCamera.GetImagePlaneHeight());
        glUniform1f(imagePlaneWidthLoc, mainCamera.GetImagePlaneWidth());
        glUniform1i(screenWidthLoc, screenWidth);
        glUniform1i(screenHeightLoc, screenHeight);

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Swap the buffers
        glfwSwapBuffers(window);

        // Poll for events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}