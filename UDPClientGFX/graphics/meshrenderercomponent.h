#pragma once

#include <common.h>

#include <glm/glm.hpp>

#include <system/component.h>

#include <GL/glew.h>

gdpNamespaceBegin

struct MeshRendererComponent : public Component
{
public:
	MeshRendererComponent() { }
	MeshRendererComponent(GLuint vbo, unsigned int numTriangles, glm::vec3 color) :vbo(vbo), numTriangles(numTriangles), color(color) { }
	GLuint vbo = 0;
	unsigned int numTriangles = 0;
	glm::vec3 color = glm::vec3(1.f);
};

gdpNamespaceEnd