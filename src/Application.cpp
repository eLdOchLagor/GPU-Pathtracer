#include "Application.h"

Application::Application(int width, int height, const std::string& title) : bvhTree(Scene{0}.primitives) {
	screenWidth = width;
	screenHeight = height;
	window = createWindow(title);
    mainCamera = Camera(vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 80.0f, screenWidth, screenHeight);
	currentScene = Scene{0};
	Init();
}

Application::~Application() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

    glfwSwapInterval(0); // Disable VSync to uncap FPS

    // Set the user pointer to the current Application instance, used to get reference to window inside static callback functions
    glfwSetWindowUserPointer(window, this);

    return window;
}

void Application::Init() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Shader initialization ----------------------------------------------------------------
    
    //Shader FragmentShader = Shader("..\\shaders\\PathtraceShaderNoBVH.frag", GL_FRAGMENT_SHADER);
    Shader PathtraceFragment = Shader("..\\shaders\\PathtraceShader.frag", GL_FRAGMENT_SHADER);
    Shader DisplayFragment = Shader("..\\shaders\\DisplayShader.frag", GL_FRAGMENT_SHADER);
    Shader DisplayVertex = Shader("..\\shaders\\DisplayShader.vert", GL_VERTEX_SHADER);
    Shader RasterVertex = Shader("..\\shaders\\RasterShader.vert", GL_VERTEX_SHADER);
    Shader RasterFragment = Shader("..\\shaders\\RasterShader.frag", GL_FRAGMENT_SHADER);

    PathtraceShader = glCreateProgram();
    glAttachShader(PathtraceShader, DisplayVertex.shaderID);
    glAttachShader(PathtraceShader, PathtraceFragment.shaderID);
    glLinkProgram(PathtraceShader);

    DisplayShader = glCreateProgram();
    glAttachShader(DisplayShader, DisplayVertex.shaderID);
    glAttachShader(DisplayShader, DisplayFragment.shaderID);
    glLinkProgram(DisplayShader);

    RasterShader = glCreateProgram();
    glAttachShader(RasterShader, RasterVertex.shaderID);
    glAttachShader(RasterShader, RasterFragment.shaderID);
    glLinkProgram(RasterShader);

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

    glGetProgramiv(RasterShader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(RasterShader, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as it is no longer needed
    glDeleteShader(DisplayVertex.shaderID);
    glDeleteShader(PathtraceFragment.shaderID);
    glDeleteShader(DisplayFragment.shaderID);
    glDeleteShader(RasterFragment.shaderID);
    glDeleteShader(RasterVertex.shaderID);

    // ----------------------------------------------------------------------------------------

    unsigned int timeLoc = glGetUniformLocation(PathtraceShader, "time");

	// Bind SSBO buffers
	BindBuffersPathtraced();
    


    glViewport(0, 0, screenWidth, screenHeight);

    // Updates glViewport when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    currentTexture = 0;
    glGenFramebuffers(1, &framebuffer);

	// ImGui Initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::Run() {
    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;

        // Rendering code
        //RenderPathtraced();

        RenderRasterized();

        // ImGui UI -------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		RenderGui(window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // ---------------------------------------------------

        // Swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        previousTime = currentTime;
        frameCount++;
    }
}

void Application::RenderRasterized()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    glClearColor(0.5, 0.5, 0.5, 1.0);

    glUseProgram(RasterShader);

    for (auto& obj : currentScene.objects) {
        obj.RenderObject();
    }
}

void Application::RenderPathtraced()
{
    int nextTexture = 1 - currentTexture;

    // Upload uniform variables to shader ---------------------------------------------------------
    uploadUniformVec3ToShader(PathtraceShader, "cameraPosition", mainCamera.GetPosition());
    uploadUniformVec3ToShader(PathtraceShader, "forward", mainCamera.GetForward());
    uploadUniformVec3ToShader(PathtraceShader, "right", mainCamera.GetRight());
    uploadUniformVec3ToShader(PathtraceShader, "up", mainCamera.GetUp());

    uploadUniformFloatToShader(PathtraceShader, "imagePlaneHeight", mainCamera.GetImagePlaneHeight());
    uploadUniformFloatToShader(PathtraceShader, "imagePlaneWidth", mainCamera.GetImagePlaneWidth());

    uploadUniformIntToShader(PathtraceShader, "screenWidth", screenWidth);
    uploadUniformIntToShader(PathtraceShader, "screenHeight", screenHeight);
    uploadUniformIntToShader(PathtraceShader, "frameCount", frameCount);

    uploadUniformIntToShader(PathtraceShader, "numberOfSamples", numberOfSamples);
    uploadUniformIntToShader(PathtraceShader, "maxBounces", maxBounces);

    uploadUniformIntToShader(PathtraceShader, "NUM_OF_POINT_LIGHTS", currentScene.pointLights.size());
    uploadUniformIntToShader(PathtraceShader, "NUM_OF_AREA_LIGHTS", currentScene.areaLights.size());
    // ----------------------------------------------------------------------------------------------

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

    currentTexture = nextTexture;
    // -----------------------------------------------------
}

void Application::RenderGui(GLFWwindow* window)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

	// Set up ImGui window position and size
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 windowSize(300, screenSize.y - 20); // Desired size

    ImVec2 pos(screenSize.x - windowSize.x - 10, 10); // 10px margin from top-right
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(windowSize);
	// -------------------------------------------------

    ImGui::Begin("User interface", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);

    if (ImGui::Button("Enable Pathtracing"))
    {
        std::cout << "Switches to raytracing";
    }
    if (ImGui::SliderInt("Samples/frame", &numberOfSamples, 1, 50))
    {
        app->frameCount = 0;
        clearAccumulationBuffer(window);
    }
    if (ImGui::SliderInt("Max number of bounces", &maxBounces, 1, 20))
    {
        app->frameCount = 0;
        clearAccumulationBuffer(window);
    }
    
    // TODO:
    // Create rastered rendering mode
    // Add object management
    // When pressing a object in the menu, give options to modify material properties, position, etc.
    // Add Glossy material (se sebastian lagues f�rsta video), g�r s� att speglar skapas genom reflektivt material
    // Add environment lighting
    // Fix glass material

	// Object management ------------------------------------------------------------
    ImGui::BeginChild("Scene Objects", ImVec2(0, 200), true);

    if (currentScene.objects.empty()) {
        ImGui::TextDisabled("No objects in the scene.");
    }
    else {
        for (int i = 0; i < currentScene.objects.size(); i++) {
            std::string label = "Object " + std::to_string(i); // Or use object names
            if (ImGui::Selectable(label.c_str(), selectedIndex == i)) {
                selectedIndex = i;
            }
        }
    }

    ImGui::EndChild();
    
    if (ImGui::Button("Add Object")) {
        Object newObj;
        currentScene.objects.push_back(newObj);
        selectedIndex = currentScene.objects.size() - 1;
    }

	// If an object is selected, show its properties
    if (selectedIndex >= 0 && selectedIndex < currentScene.objects.size()) 
    {
        Object& obj = currentScene.objects[selectedIndex];
        
        ImGui::Separator();
        ImGui::Text("Selected Object %d", selectedIndex);

        // Load model from disk ------------------------------------------------------------------------
        if (ImGui::Button("Load .OBJ file")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", config);
        }
        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

                obj.CreateObjectFromModel(filePathName);
				obj.SetName(fileName);

				obj.BindBuffers();
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
		// --------------------------------------------------------------------------------------------
        
        ImGui::Text("Transform");
        ImGui::DragFloat3("Position", &obj.position.x, 0.01f);
        ImGui::DragFloat3("Rotation", &obj.rotation.x, 0.5f);
        ImGui::DragFloat3("Scale", &obj.scale.x, 0.01f);
    }
	// ----------------------------------------------------------------------------
    
    ImGui::End();
}

void Application::BindBuffersPathtraced()
{
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
    GLuint SSBO_BVH;
    GLuint SSBO_Indices;
    GLuint SSBO_PointLights;
    GLuint SSBO_AreaLights;

    std::vector<BVHNode> gpuNodes;
    gpuNodes.reserve(bvhTree.getNodes().size());
    for (const BVHNode& node : bvhTree.getNodes()) {
        gpuNodes.push_back({
            node.bBoxMin,
            node.leftChild,
            node.bBoxMax,
            node.rightChild,
            node.startTriangle,
            node.triangleCount,
            node.escapeIndex,0
            });
    }

    // Allocate SSBO for spheres

    std::cout << "Size: " << sizeof(Primitive) << " Bytes \n";

    glGenBuffers(1, &SSBO_Primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Primitives);
    glBufferData(GL_SHADER_STORAGE_BUFFER, currentScene.primitives.size() * sizeof(Primitive), currentScene.primitives.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO_Primitives);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &SSBO_BVH);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_BVH);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gpuNodes.size() * sizeof(BVHNode), gpuNodes.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO_BVH);

    glGenBuffers(1, &SSBO_Indices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Indices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvhTree.getIndices().size() * sizeof(int), bvhTree.getIndices().data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO_Indices);

    glGenBuffers(1, &SSBO_PointLights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_PointLights);
    glBufferData(GL_SHADER_STORAGE_BUFFER, currentScene.pointLights.size() * sizeof(PointLight), currentScene.pointLights.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBO_PointLights);

    glGenBuffers(1, &SSBO_AreaLights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_AreaLights);
    glBufferData(GL_SHADER_STORAGE_BUFFER, currentScene.areaLights.size() * sizeof(AreaLight), currentScene.areaLights.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, SSBO_AreaLights);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Application::BindBuffersRasterized()
{
	for (auto& obj : currentScene.objects) {
		obj.BindBuffers();
	}
}

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (app->mainCamera.cameraEnabled)
    {
        float xoffset = xpos - app->mainCamera.lastX;
        float yoffset = app->mainCamera.lastY - ypos; // reversed since y-coordinates range from bottom to top
        app->mainCamera.lastX = xpos;
        app->mainCamera.lastY = ypos;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        app->mainCamera.yaw += xoffset;
        app->mainCamera.pitch += yoffset;

        if (app->mainCamera.pitch > 89.0f)
            app->mainCamera.pitch = 89.0f;
        if (app->mainCamera.pitch < -89.0f)
            app->mainCamera.pitch = -89.0f;

        vec3 forwardDirection = vec3(0.0f);
        // As with wasd movement, left and right are opposite
        forwardDirection.z = cos(app->mainCamera.yaw * M_PI / 180.0f) * cos(app->mainCamera.pitch * M_PI / 180.0f);
        forwardDirection.y = sin(app->mainCamera.pitch * M_PI / 180.0f);
        forwardDirection.x = sin(app->mainCamera.yaw * M_PI / 180.0f) * cos(app->mainCamera.pitch * M_PI / 180.0f);

        app->mainCamera.SetForward(forwardDirection);

        // Reset accumulation
        app->frameCount = 0;
        clearAccumulationBuffer(window);
    }
}

void Application::processInput(GLFWwindow* window)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    bool cameraMoved = false;
    float cameraSpeed = 6.0f * app->deltaTime;

    // NOTE: Left and right are reversed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        app->mainCamera.position += cameraSpeed * app->mainCamera.GetForward();
        cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        app->mainCamera.position -= cameraSpeed * app->mainCamera.GetForward();
        cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        app->mainCamera.position += app->mainCamera.GetRight() * cameraSpeed;
        cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        app->mainCamera.position -= app->mainCamera.GetRight() * cameraSpeed;
        cameraMoved = true;
    }

    if (cameraMoved)
    {
        app->frameCount = 0;
        clearAccumulationBuffer(window);
    }
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        app->mainCamera.cameraEnabled = !app->mainCamera.cameraEnabled;
        if (app->mainCamera.cameraEnabled)
        {
            //Capture mouse
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        
    }

}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    if (yoffset != 0.0)
    {
        float currentFOV = app->mainCamera.GetFOV();
        app->mainCamera.SetFOV(currentFOV - yoffset);

        // Redo these checks, it still resets accumulation currently, make sure it doesnt
        if (app->mainCamera.GetFOV() > 180.0f)
        {
            app->mainCamera.SetFOV(179.0f);
            return;
        }
        if (app->mainCamera.GetFOV() < 0.0f)
        {
            app->mainCamera.SetFOV(1.0f);
            return;
        }

        app->frameCount = 0;
        clearAccumulationBuffer(window);

    }
    
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::clearAccumulationBuffer(GLFWwindow* window) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    glBindFramebuffer(GL_FRAMEBUFFER, app->framebuffer);

    // Bind both textures and clear them
    for (int i = 0; i < 2; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, app->textures[i], 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Reset to black
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}