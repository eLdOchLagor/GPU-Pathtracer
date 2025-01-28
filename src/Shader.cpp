#include "Shader.h"

std::string Shader::readShaderFile(const char* filePath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    // Open the file
    shaderFile.open(filePath);
    if (!shaderFile.is_open())
    {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    // Read the file into a string stream
    shaderStream << shaderFile.rdbuf();

    // Close the file
    shaderFile.close();

    // Convert the stream into a string and return
    return shaderStream.str();
}

unsigned int Shader::compileShader(const char* shaderSource, GLenum shaderType) {
    // Create the shader
    unsigned int shader = glCreateShader(shaderType);

    // Attach the source code and compile
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}