#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"



#define MAIN

#include "VectorUtils4.h"





void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int screenWidth = 800;
int screenHeight = 600;

int frameCount = 0;

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
    
    Camera mainCamera = Camera(vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 80.0f, screenWidth, screenHeight);

    Scene roomScene{0};
    
    
    /*triangles[MAX_TRIANGLES_FOR_ROOM].vertex1 = vec3(-0.5f, -0.5f, 0.5f);
    triangles[MAX_TRIANGLES_FOR_ROOM].vertex2 = vec3(0.5f, -0.5f, 0.5f,0.0f);
    triangles[MAX_TRIANGLES_FOR_ROOM].vertex3 = vec3(0.0f, 0.5f, 0.5f);
    triangles[MAX_TRIANGLES_FOR_ROOM].normal = vec3(0.0f, 0.0f, -1.0f);*/

    glfwInit();
    float previousTime = 0;
    float deltaTime = 0;

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
    Shader DisplayFragment = Shader("..\\shaders\\DisplayShader.frag", GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, VertexShader.shaderID);
    glAttachShader(shaderProgram, FragmentShader.shaderID);
    glLinkProgram(shaderProgram);

    unsigned int DisplayShader = glCreateProgram();
    glAttachShader(DisplayShader, VertexShader.shaderID);
    glAttachShader(DisplayShader, DisplayFragment.shaderID);
    glLinkProgram(DisplayShader);

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

    
    // Texture for storing accumulated color
    GLuint textures[2];
    glGenTextures(2, textures);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    // --------------------------------------------------------------------

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //ubos instead of SSBO since were at an earlier version of opengl // JONATANS EXTRA FINA TESTKOD
    GLuint SSBO_Primitives;

    // Allocate SSBO for spheres

    std::cout << "Size: " << sizeof(Primitive) << " Bytes \n";

    glGenBuffers(1, &SSBO_Primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Primitives);
    glBufferData(GL_SHADER_STORAGE_BUFFER, roomScene.primitives.size() * sizeof(Primitive), roomScene.primitives.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO_Primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    glViewport(0, 0, screenWidth, screenHeight);

    // Updates glViewport when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int currentTexture = 0;
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        int nextTexture = 1 - currentTexture;

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;

        std::clog  << "\rFPS: " << 1 / deltaTime;

        /*
        for (int i = 24; i < MAX_PRIMITVES; i++) {
            primitives[i].ID = 1;
            primitives[i].color = vec3(1.0f, 1.0f, 0.0f);
            primitives[i].vertex1 = vec3(0.0f, 0.0f, 6.0f) + vec3(3*sin(currentTime), 3*cos(currentTime), 5*cos(currentTime));
            primitives[i].vertex2 = vec3(1.2f, 0.0f, 0.0f);
            primitives[i].bounceOdds = 1.0f;
        }
        */

        uploadUniformVec3ToShader(shaderProgram, "cameraPosition", mainCamera.GetPosition());
        uploadUniformVec3ToShader(shaderProgram, "forward", mainCamera.GetForward());
        uploadUniformVec3ToShader(shaderProgram, "right", mainCamera.GetRight());
        uploadUniformVec3ToShader(shaderProgram, "up", mainCamera.GetUp());

        uploadUniformFloatToShader(shaderProgram, "imagePlaneHeight", mainCamera.GetImagePlaneHeight());
        uploadUniformFloatToShader(shaderProgram, "imagePlaneWidth", mainCamera.GetImagePlaneWidth());

        uploadUniformIntToShader(shaderProgram, "screenWidth", screenWidth);
        uploadUniformIntToShader(shaderProgram, "screenHeight", screenHeight);
        uploadUniformIntToShader(shaderProgram, "frameCount", frameCount);

        /*
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Primitives);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (MAX_PRIMITVES) * sizeof(Primitive), primitives);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        */

        // Pathtracing pass, Rendering to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[nextTexture], 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram); // Pathtracing shader

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer to render to screen
        // -------------------------------------------------------------------------------

        // Display pass, render accumulated image to screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(DisplayShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[nextTexture]); // Bind the accumulated result

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // -----------------------------------------------------

        
        // Swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        previousTime = currentTime;
        currentTexture = nextTexture;
        frameCount++;
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}