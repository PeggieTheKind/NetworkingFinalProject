#pragma once

#include <common.h>

#include <system/component.h>

#include <GL/glew.h>

gdpNamespaceBegin

class NetComponent : public Component
{
public:
	NetComponent(bool owner) : IsOwner(owner) { }

	bool IsOwner = false;

};

gdpNamespaceEnd