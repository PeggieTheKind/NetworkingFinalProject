#pragma once

#include <common.h>

#include <GL/glew.h>

gdpNamespaceBegin

struct Shader {
	std::string filename;
	std::string source;
	bool bIsCompiled;
	GLuint id;
};
gdpNamespaceEnd