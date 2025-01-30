#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

struct Sphere {
    glm::vec3 position;
    float radius;
};

struct Triangle {
    glm::vec4 vertex1;
    glm::vec4 vertex2;
    glm::vec4 vertex3;
    glm::vec4 normal;
    glm::vec4 color;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

void getRoom(Triangle triangles[]) {
    glm::vec3 e1 = glm::vec3(0.0f);
    glm::vec3 e2 = glm::vec3(0.0f);
    int i = 0;
    //floor
        //triangle front
        triangles[i].vertex1 = glm::vec4(0.0f, -5.0f, -3.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(38.0f / 255, 156.0f / 255, 169.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //triangle mid right
        triangles[i].vertex1 = glm::vec4(6.0f, -5.0f, 0.0f,0.0f);
        triangles[i].vertex2 = glm::vec4(6.0f, -5.0f, 10.0f,0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, -5.0f, 10.0f,0.0f);
        triangles[i].normal = glm::vec4(0.0f,1.0f,0.0f,0.0f);
        triangles[i].color = glm::vec4(38.0f / 255, 156.0f / 255, 169.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //triangle mid left
        triangles[i].vertex1 = glm::vec4(6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(38.0f / 255, 156.0f / 255, 169.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //triangle back
        triangles[i].vertex1 = glm::vec4(0.0f, -5.0f, 13.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(38.0f / 255, 156.0f / 255, 169.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
    //backroom walls
        //leftwall back
        triangles[i].vertex1 = glm::vec4(-6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(0.0f, -5.0f, 13.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(0.0f, 5.0f, 13.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)),0.0f);
        triangles[i].color = glm::vec4(234.0f/255, 215.0f/255, 165.0f/255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //leftwall front
        triangles[i].vertex1 = glm::vec4(-6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(0.0f, 5.0f, 13.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, 5.0f, 10.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
    //backroom walls
        //leftwall back
        triangles[i].vertex1 = glm::vec4(6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(0.0f, 5.0f, 13.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(0.0f, -5.0f, 13.0f, 0.0f);      
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //leftwall front
        triangles[i].vertex1 = glm::vec4(6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(0.0f, 5.0f, 13.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
    //right walls 
        //bottom wall
        triangles[i].vertex1 = glm::vec4(6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, -5.0f, 10.0f,0.0f);
        triangles[i].normal = glm::vec4(-1.0f,0.0f,0.0f,0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //top wall
        triangles[i].vertex1 = glm::vec4(6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;

    //left walls 
        //bottom wall
        triangles[i].vertex1 = glm::vec4(-6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //top wall
        triangles[i].vertex1 = glm::vec4(-6.0f, -5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
    //frontroom walls
        //left bottom
        triangles[i].vertex1 = glm::vec4(0.0f, -5.0f, -3.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(0.0f, 5.0f, -3.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //left top
        triangles[i].vertex1 = glm::vec4(-6.0f, -5.0f, 0.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(0.0f, 5.0f, -3.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
    //frontroom walls
        //right bottom
        triangles[i].vertex1 = glm::vec4(0.0f, -5.0f, -3.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(0.0f, 5.0f, -3.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, -5.0f, 0.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //right top
        triangles[i].vertex1 = glm::vec4(0.0f, 5.0f, -3.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, -5.0f, 0.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = glm::vec4(glm::normalize(glm::cross(e2, e1)), 0.0f);
        triangles[i].color = glm::vec4(234.0f / 255, 215.0f / 255, 165.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
    //roof
        //triangle front
        triangles[i].vertex1 = glm::vec4(0.0f, 5.0f, -3.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(163.0f / 255, 211.0f / 255, 214.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //triangle mid right
        triangles[i].vertex1 = glm::vec4(6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(163.0f / 255, 211.0f / 255, 214.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //triangle mid left
        triangles[i].vertex1 = glm::vec4(6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(-6.0f, 5.0f, 0.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(-6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(163.0f / 255, 211.0f / 255, 214.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
        i++;
        //triangle back
        triangles[i].vertex1 = glm::vec4(-6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].vertex2 = glm::vec4(0.0f, 5.0f, 13.0f, 0.0f);
        triangles[i].vertex3 = glm::vec4(6.0f, 5.0f, 10.0f, 0.0f);
        triangles[i].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        triangles[i].color = glm::vec4(163.0f / 255, 211.0f / 255, 214.0f / 255, 0.0f) + glm::vec1(4.f * i / 255.f);
}

int main() {
    const int MAX_SPHERES = 1;
    const int MAX_TRIANGLES = 0;
    const int MAX_TRIANGLES_FOR_ROOM = 24;
    Sphere spheres[MAX_SPHERES];
    Triangle triangles[MAX_TRIANGLES + MAX_TRIANGLES_FOR_ROOM];
    getRoom(triangles);
    /*triangles[MAX_TRIANGLES_FOR_ROOM].vertex1 = glm::vec4(-0.5f, -0.5f, 0.5f, 0.0f);
    triangles[MAX_TRIANGLES_FOR_ROOM].vertex2 = glm::vec4(0.5f, -0.5f, 0.5f,0.0f);
    triangles[MAX_TRIANGLES_FOR_ROOM].vertex3 = glm::vec4(0.0f, 0.5f, 0.5f, 0.0f);
    triangles[MAX_TRIANGLES_FOR_ROOM].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);*/

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

    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //ubos instead of SSBO since were at an earlier version of opengl // JONATANS EXTRA FINA TESTKOD
    GLuint SSBO_Spheres, SSBO_Triangles;

    // Allocate SSBO for spheres
    glGenBuffers(1, &SSBO_Spheres);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Spheres);
    glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_SPHERES * sizeof(Sphere), spheres, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO_Spheres);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Allocate SSBO for triangles
    glGenBuffers(1, &SSBO_Triangles);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Triangles);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (MAX_TRIANGLES + MAX_TRIANGLES_FOR_ROOM) * sizeof(Triangle), triangles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO_Triangles);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    glViewport(0, 0, screenWidth, screenHeight);

    // Updates glViewport when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - previousTime;

        std::clog  << "\rFPS: " << 1 / deltaTime;
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

        for (int i = 0; i < MAX_SPHERES; i++) {
            spheres[i].position = glm::vec3(0.0f,0.0f, 10.0f);
            spheres[i].radius = 0.5f;
           
        }
        

        // Update spheres SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Spheres);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, MAX_SPHERES * sizeof(Sphere), spheres);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Update triangles SSBO (offset must be 0, because it's a separate buffer)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Triangles);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, MAX_TRIANGLES * sizeof(Triangle), triangles);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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
        previousTime = currentFrame;
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}