#include "Application.h"

Application::Application(int width, int height, const std::string& title) {
	screenWidth = width;
	screenHeight = height;
	window = createWindow(title);
    mainCamera = Camera(vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 80.0f, screenWidth, screenHeight);
	roomScene = Scene{ 0 };

	Init();
}

Application::~Application() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* Application::createWindow(const std::string& title) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    return window;
}

void Application::Init() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // Shader initialization ----------------------------------------------------------------
    Shader VertexShader = Shader("..\\shaders\\VertexShader.vert", GL_VERTEX_SHADER);
    Shader FragmentShader = Shader("..\\shaders\\FragmentShader.frag", GL_FRAGMENT_SHADER);
    Shader DisplayFragment = Shader("..\\shaders\\DisplayShader.frag", GL_FRAGMENT_SHADER);

    PathtraceShader = glCreateProgram();
    glAttachShader(PathtraceShader, VertexShader.shaderID);
    glAttachShader(PathtraceShader, FragmentShader.shaderID);
    glLinkProgram(PathtraceShader);

    DisplayShader = glCreateProgram();
    glAttachShader(DisplayShader, VertexShader.shaderID);
    glAttachShader(DisplayShader, DisplayFragment.shaderID);
    glLinkProgram(DisplayShader);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(PathtraceShader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(PathtraceShader, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glGetProgramiv(DisplayShader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(DisplayShader, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as it is no longer needed
    glDeleteShader(VertexShader.shaderID);
    glDeleteShader(FragmentShader.shaderID);
    glDeleteShader(DisplayFragment.shaderID);

    // ----------------------------------------------------------------------------------------

    unsigned int timeLoc = glGetUniformLocation(PathtraceShader, "time");


    glGenTextures(2, textures);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    // --------------------------------------------------------------------

    unsigned int VBO;
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    currentTexture = 0;
    glGenFramebuffers(1, &framebuffer);
}

void Application::Run() {
    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        //processInput(window);

        int nextTexture = 1 - currentTexture;

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;

        std::clog << "\rFPS: " << 1 / deltaTime;

        /*
        for (int i = 24; i < MAX_PRIMITVES; i++) {
            primitives[i].ID = 1;
            primitives[i].color = vec3(1.0f, 1.0f, 0.0f);
            primitives[i].vertex1 = vec3(0.0f, 0.0f, 6.0f) + vec3(3*sin(currentTime), 3*cos(currentTime), 5*cos(currentTime));
            primitives[i].vertex2 = vec3(1.2f, 0.0f, 0.0f);
            primitives[i].bounceOdds = 1.0f;
        }
        */

        uploadUniformVec3ToShader(PathtraceShader, "cameraPosition", mainCamera.GetPosition());
        uploadUniformVec3ToShader(PathtraceShader, "forward", mainCamera.GetForward());
        uploadUniformVec3ToShader(PathtraceShader, "right", mainCamera.GetRight());
        uploadUniformVec3ToShader(PathtraceShader, "up", mainCamera.GetUp());

        uploadUniformFloatToShader(PathtraceShader, "imagePlaneHeight", mainCamera.GetImagePlaneHeight());
        uploadUniformFloatToShader(PathtraceShader, "imagePlaneWidth", mainCamera.GetImagePlaneWidth());

        uploadUniformIntToShader(PathtraceShader, "screenWidth", screenWidth);
        uploadUniformIntToShader(PathtraceShader, "screenHeight", screenHeight);
        uploadUniformIntToShader(PathtraceShader, "frameCount", frameCount);

        /*
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Primitives);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (MAX_PRIMITVES) * sizeof(Primitive), primitives);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        */

        // Pathtracing pass, Rendering to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[nextTexture], 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(PathtraceShader); // Pathtracing shader

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
}

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    /*
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
    forwardDirection.z = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
    forwardDirection.y = sin(pitch * M_PI / 180.0f);
    forwardDirection.x = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);

    mainCamera.SetForward(forwardDirection);

    // Reset accumulation
    frameCount = 0;
    clearAccumulationBuffer();
    */
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	/*
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
    */
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::clearAccumulationBuffer() {
    /*
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Bind both textures and clear them
    for (int i = 0; i < 2; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Reset to black
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
}