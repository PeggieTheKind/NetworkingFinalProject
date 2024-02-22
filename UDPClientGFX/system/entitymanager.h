#pragma once

#include <common.h>

#include <system/component.h>
#include <system/entity.h>

gdpNamespaceBegin

class EntityManager
{
public:
	static EntityManager& GetInstance();

	void GetEntities(std::vector<Entity*>& entities) const;
	Entity* CreateEntity();
	void RemoveEntity(Entity* entity);

private:   
    EntityManager();
    ~EntityManager();

    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    std::vector<Entity*> m_Entities;
};

#define GetEntityManager() EntityManager::GetInstance()

gdpNamespaceEnd