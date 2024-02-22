#pragma once
#include <common.h>

#include <glm/glm.hpp>

#include <system/component.h>

gdpNamespaceBegin
struct RotateComponent : public Component
{
public:
	RotateComponent() { }
	RotateComponent(glm::vec3& angle, float speed) : angle(angle), speed(speed) { }
	glm::vec3 angle = glm::vec3(0.f);
	float speed = 0.f;
};
gdpNamespaceEnd
