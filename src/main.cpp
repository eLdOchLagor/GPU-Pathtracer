#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"


#define MAIN

#include "VectorUtils4.h"


struct Primitive {
    alignas(16) vec3 vertex1;
    alignas(16) vec3 vertex2;
    alignas(16) vec3 vertex3;
    alignas(16) vec3 color;
    alignas(16) vec3 normal;
    alignas(4) int ID; // 0 == Triangle, 1 == Sphere
    alignas(4) float bounceOdds; //Odds that the ray would bounce off of the surface.
    alignas(8) char padding;
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int screenWidth = 800;
int screenHeight = 600;

Camera mainCamera = Camera(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 80.0f, screenWidth, screenHeight);

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

void getRoom(Primitive triangles[]) {
    vec3 e1 = vec3(0.0f);
    vec3 e2 = vec3(0.0f);
    int i = 0;
    //floor
        //triangle front
        triangles[i].vertex1 = vec3(0.0f, -5.0f, -3.0f);
        triangles[i].vertex2 = vec3(6.0f, -5.0f, 0.0f);
        triangles[i].vertex3 = vec3(-6.0f, -5.0f, 0.0f);
        triangles[i].normal = vec3(0.0f, 1.0f, 0.0f);
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //triangle mid right
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
        triangles[i].vertex2 = vec3(6.0f, -5.0f, 10.0f);
        triangles[i].vertex3 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].normal = vec3(0.0f,1.0f,0.0f);
        triangles[i].color = vec3(0.0, 1.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //triangle mid left
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
        triangles[i].vertex2 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].vertex3 = vec3(-6.0f, -5.0f, 0.0f);
        triangles[i].normal = vec3(0.0f, 1.0f, 0.0f);
        triangles[i].color = vec3(0.0, 0.0, 1.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //triangle back
        triangles[i].vertex1 = vec3(0.0f, -5.0f, 13.0f);
        triangles[i].vertex2 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].vertex3 = vec3(6.0f, -5.0f, 10.0f);
        triangles[i].normal = vec3(0.0f, 1.0f, 0.0f);
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
    //backroom walls
        //leftwall back
        triangles[i].vertex1 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].vertex2 = vec3(0.0f, -5.0f, 13.0f);
        triangles[i].vertex3 = vec3(0.0f, 5.0f, 13.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(1.0, 1.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 0.0f;
        i++;
        //leftwall front
        triangles[i].vertex1 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].vertex2 = vec3(0.0f, 5.0f, 13.0f);
        triangles[i].vertex3 = vec3(-6.0f, 5.0f, 10.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(1.0, 1.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 0.0f;
        i++;
    //backroom walls
        //leftwall back
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 10.0f);
        triangles[i].vertex2 = vec3(0.0f, 5.0f, 13.0f);
        triangles[i].vertex3 = vec3(0.0f, -5.0f, 13.0f);      
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(0.0, 1.0, 1.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //leftwall front
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 10.0f);
        triangles[i].vertex2 = vec3(6.0f, 5.0f, 10.0f);
        triangles[i].vertex3 = vec3(0.0f, 5.0f, 13.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(0.0, 1.0, 1.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
    //right walls 
        //bottom wall
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
        triangles[i].vertex2 = vec3(6.0f, 5.0f, 0.0f);
        triangles[i].vertex3 = vec3(6.0f, -5.0f, 10.0f);
        triangles[i].normal = vec3(-1.0f,0.0f,0.0f);
        triangles[i].color = vec3(0.0, 0.5, 1.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //top wall
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 10.0f);
        triangles[i].vertex2 = vec3(6.0f, 5.0f, 0.0f);
        triangles[i].vertex3 = vec3(6.0f, 5.0f, 10.0f);
        triangles[i].normal = vec3(-1.0f, 0.0f, 0.0f);
        triangles[i].color = vec3(0.0, 0.5, 1.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;

    //left walls 
        //bottom wall
        triangles[i].vertex1 = vec3(-6.0f, -5.0f, 0.0f);
        triangles[i].vertex2 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].vertex3 = vec3(-6.0f, 5.0f, 0.0f);
        triangles[i].normal = vec3(1.0f, 0.0f, 0.0f);
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //top wall
        triangles[i].vertex1 = vec3(-6.0f, -5.0f, 10.0f);
        triangles[i].vertex2 = vec3(-6.0f, 5.0f, 10.0f);
        triangles[i].vertex3 = vec3(-6.0f, 5.0f, 0.0f);
        triangles[i].normal = vec3(1.0f, 0.0f, 0.0f);
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
    //frontroom walls
        //left bottom
        triangles[i].vertex1 = vec3(0.0f, -5.0f, -3.0f);
        triangles[i].vertex2 = vec3(-6.0f, -5.0f, 0.0f);
        triangles[i].vertex3 = vec3(0.0f, 5.0f, -3.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //left top
        triangles[i].vertex1 = vec3(-6.0f, -5.0f, 0.0f);
        triangles[i].vertex2 = vec3(-6.0f, 5.0f, 0.0f);
        triangles[i].vertex3 = vec3(0.0f, 5.0f, -3.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
    //frontroom walls
        //right bottom
        triangles[i].vertex1 = vec3(0.0f, -5.0f, -3.0f);
        triangles[i].vertex2 = vec3(0.0f, 5.0f, -3.0f);
        triangles[i].vertex3 = vec3(6.0f, -5.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(1.0, 1.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //right top
        triangles[i].vertex1 = vec3(6.0f, -5.0f, 0.0f);
        triangles[i].vertex2 = vec3(0.0f, 5.0f, -3.0f);
        triangles[i].vertex3 = vec3(6.0f, 5.0f, 0.0f);
        e1 = triangles[i].vertex2 - triangles[i].vertex1;
        e2 = triangles[i].vertex3 - triangles[i].vertex2;
        triangles[i].normal = vec3(normalize(cross(e2, e1)));
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
    //roof
        //triangle front
        triangles[i].vertex1 = vec3(0.0f, 5.0f, -3.0f);
        triangles[i].vertex2 = vec3(-6.0f, 5.0f, 0.0f);
        triangles[i].vertex3 = vec3(6.0f, 5.0f, 0.0f);
        triangles[i].normal = vec3(0.0f, -1.0f, 0.0f);
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //triangle mid right
        triangles[i].vertex1 = vec3(6.0f, 5.0f, 0.0f);
        triangles[i].vertex2 = vec3(-6.0f, 5.0f, 10.0f);
        triangles[i].vertex3 = vec3(6.0f, 5.0f, 10.0f);
        triangles[i].normal = vec3(0.0f, -1.0f, 0.0f);
        triangles[i].color = vec3(0.0, 0.0, 1.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //triangle mid left
        triangles[i].vertex1 = vec3(6.0f, 5.0f, 0.0f);
        triangles[i].vertex2 = vec3(-6.0f, 5.0f, 0.0f);
        triangles[i].vertex3 = vec3(-6.0f, 5.0f, 10.0f);
        triangles[i].normal = vec3(0.0f, -1.0f, 0.0f);
        triangles[i].color = vec3(0.0, 1.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
        i++;
        //triangle back
        triangles[i].vertex1 = vec3(-6.0f, 5.0f, 10.0f);
        triangles[i].vertex2 = vec3(0.0f, 5.0f, 13.0f);
        triangles[i].vertex3 = vec3(6.0f, 5.0f, 10.0f);
        triangles[i].normal = vec3(0.0f, -1.0f, 0.0f);
        triangles[i].color = vec3(1.0, 0.0, 0.0) + (4.f * i / 255.f);
        triangles[i].ID = 0;
        triangles[i].bounceOdds = 1.0f;
}

int main() {
    
    const int MAX_PRIMITVES = 100;

    Primitive primitives[MAX_PRIMITVES];
    getRoom(primitives);
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
    glBufferData(GL_SHADER_STORAGE_BUFFER, (MAX_PRIMITVES) * sizeof(Primitive), primitives, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO_Primitives);
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
        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;

        std::clog  << "\rFPS: " << 1 / deltaTime;
        // Pass Uniforms
        glUniform1f(timeLoc, currentTime);

    
        uploadUniformVec3ToShader(shaderProgram, "cameraPosition", mainCamera.GetPosition());
        uploadUniformVec3ToShader(shaderProgram, "forward", mainCamera.GetForward());
        uploadUniformVec3ToShader(shaderProgram, "right", mainCamera.GetRight());
        uploadUniformVec3ToShader(shaderProgram, "up", mainCamera.GetUp());

        uploadUniformFloatToShader(shaderProgram, "imagePlaneHeight", mainCamera.GetImagePlaneHeight());
        uploadUniformFloatToShader(shaderProgram, "imagePlaneWidth", mainCamera.GetImagePlaneWidth());

        uploadUniformIntToShader(shaderProgram, "screenWidth", screenWidth);
        uploadUniformIntToShader(shaderProgram, "screenHeight", screenHeight);

        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Primitives);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (MAX_PRIMITVES) * sizeof(Primitive), primitives);
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
        previousTime = currentTime;
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}