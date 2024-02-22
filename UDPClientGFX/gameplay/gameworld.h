#pragma once

#include <system/entity.h>

#include <common.h>

gdpNamespaceBegin

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void StartUp();
	void Shutdown();

	void Update();

private:
	std::vector<Entity*> m_Entities;
};

gdpNamespaceEnd