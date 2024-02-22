#pragma once

#include <common.h>

#include <system/system.h>
#include <system/entity.h>


gdpNamespaceBegin
class PlayerMovementSystem : public System
{
public:
	void Execute(const std::vector<Entity*>& entities, float dt) override;
};
gdpNamespaceEnd;