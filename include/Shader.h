#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

class Shader
{
public:
	Shader(const char* filePath, GLenum shaderType) {
		shaderCode = readShaderFile(filePath);
		shaderID = compileShader(shaderCode.c_str(), shaderType);
	}

	unsigned int shaderID;

private:
	std::string readShaderFile(const char* filePath);
	unsigned int compileShader(const char* shaderSource, GLenum shaderType);

	std::string shaderCode;
};
