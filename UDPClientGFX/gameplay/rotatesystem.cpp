#include <gameplay/rotatesystem.h>

#include <gameplay/rotatecomponent.h>
#include <graphics/transformcomponent.h>

gdpNamespaceBegin
void RotateSystem::Execute(const std::vector<Entity*>& entities, float dt) {
	for (int i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];

		if (entity->HasComponent<RotateComponent>() && entity->HasComponent<TransformComponent>())
		{
			TransformComponent* transform = entity->GetComponent<TransformComponent>();
			RotateComponent* rotate = entity->GetComponent<RotateComponent>();

		}
	}
}
gdpNamespaceEnd