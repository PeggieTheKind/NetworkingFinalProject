#pragma once

#include <common.h>

#include <graphics/shader.h>

#include <system/component.h>

#include <GL/glew.h>

gdpNamespaceBegin

class ShaderProgram {
public:
	ShaderProgram(const char* vertexFilepath, const char* fragmentFilepath);
	~ShaderProgram();

	std::string name;
	Shader vertexShader;
	Shader fragmentShader;
	GLuint id;
};
gdpNamespaceEnd