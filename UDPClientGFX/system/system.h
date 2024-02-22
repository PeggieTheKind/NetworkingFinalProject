#pragma once

#include <common.h>
#include <system/entity.h>

gdpNamespaceBegin

class System
{
public:
	System() = default;
	virtual ~System() { }

	virtual void Execute(const std::vector<Entity*>& entities, float dt) = 0;

protected:
};

gdpNamespaceEnd