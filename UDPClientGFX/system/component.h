#pragma once

#include <common.h>

gdpNamespaceBegin

class Component
{
public:
	virtual ~Component() { }

protected:
	Component() = default;
};

gdpNamespaceEnd