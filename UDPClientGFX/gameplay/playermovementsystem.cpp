#include <gameplay/playermovementsystem.h>

#include <gameplay/playercontrollercomponent.h>
#include <graphics/transformcomponent.h>

gdpNamespaceBegin

void PlayerMovementSystem::Execute(const std::vector<Entity*>& entities, float dt)
{
	PlayerControllerComponent* controller = nullptr;
	TransformComponent* transform = nullptr;

	const float speed = 7.f;

	for (int i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];
		controller = entity->GetComponent<PlayerControllerComponent>();
		transform = entity->GetComponent<TransformComponent>();


		if (controller != nullptr && transform != nullptr)
		{
			glm::vec3 direction = glm::vec3(0.f);

			
			direction.x += controller->moveForward ? 1.f : 0.f;
			direction.x += controller->moveBackward ? -1.f : 0.f;
			transform->position += direction * speed * dt;

			transform->orientation.y += controller->moveLeft ? -0.01f : 0.f;
			transform->orientation.y += controller->moveRight ? 0.01f : 0.f;

			if (transform->orientation.y > .6)
				transform->orientation.y = .6;

			if (transform->orientation.y < -.6)
				transform->orientation.y = -.6;

		}
	}
}

gdpNamespaceEnd