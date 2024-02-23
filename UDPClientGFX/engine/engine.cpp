#include <engine/engine.h>

#include <sstream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <gameplay/playercontrollercomponent.h>
#include <gameplay/rotatecomponent.h>

#include <graphics/meshrenderercomponent.h>
#include <graphics/transformcomponent.h>

#include <system/entitymanager.h>
#include <network/netcomponent.h>

gdpNamespaceBegin

void _CheckGLError(const char* file, int line)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;
		switch (err)
		{
		case GL_INVALID_OPERATION:				error = "INVALID_OPERATION";				break;
		case GL_INVALID_ENUM:					error = "INVALID_ENUM";					    break;
		case GL_INVALID_VALUE:					error = "INVALID_VALUE";					break;
		case GL_OUT_OF_MEMORY:					error = "OUT_OF_MEMORY";					break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";    break;
		}
		std::stringstream ss;
		ss << "[" << file << ":" << line << "] " << error;

		printf("OpenGL Error: %s\n", ss.str().c_str());

		err = glGetError();
	}

	return;
}
Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::Initialize()
{
	LoadAssets();

	m_GameWorld = new GameWorld();
	m_GameWorld->StartUp();

	m_NetworkManager.Initialize();

	m_LastTime = std::chrono::high_resolution_clock::now();
}

void Engine::Destroy()
{
	m_GameWorld->Shutdown();
	m_NetworkManager.Destroy();
}

void Engine::Resize(int w, int h)
{
	if (w <= 0 || h <= 0) return;
	m_WindowWidth = w;
	m_WindowHeight = h;
	m_WindowRatio = static_cast<float>(w) / static_cast<float>(h);
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
}

void Engine::Update()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt = currentTime - m_LastTime;
	m_LastTime = currentTime;

	//m_GameWorld->Update();

	// Handle user Input
	PlayerControllerComponent* controller = m_Player->GetComponent<PlayerControllerComponent>();
	controller->moveBackward = m_Keys['s'];
	controller->moveForward = m_Keys['w'];
	controller->moveLeft = m_Keys['a'];
	controller->moveRight = m_Keys['d'];
	controller->hasShot = m_Keys['q'];

	std::vector<Entity*> entities;
	GetEntityManager().GetEntities(entities);

	for (int i = 0; i < m_Systems.size(); i++)
	{
		m_Systems[i]->Execute(entities, dt.count());
	}

	TransformComponent* transform = m_Player->GetComponent<TransformComponent>();

	m_NetworkManager.SendPlayerPositionToServer(transform->position.x, transform->position.z, transform->orientation.x, transform->orientation.z, controller->hasShot);
	m_NetworkManager.Update();

	for (int i = 0; i < 4; i++)
	{
		TransformComponent* transform = m_NetworkedEntities[i]->GetComponent<TransformComponent>();
		transform->position.x = m_NetworkManager.m_NetworkedPositions[i].x;
		transform->position.z = m_NetworkManager.m_NetworkedPositions[i].z;
		transform->orientation.x = m_NetworkManager.m_NetworkedPositions[i].l;
		transform->orientation.z = m_NetworkManager.m_NetworkedPositions[i].r;

	}
}

void Engine::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::vec3 up(0.f, 1.f, 0.f);
	const glm::vec3 forward(0.f, 0.f, -1.f);

	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(45.0f),
		((GLfloat)m_WindowWidth) / ((GLfloat)m_WindowHeight),
		0.1f,
		10000.0f
	);
	glUniformMatrix4fv(m_ProjectionMatrixUL, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	CheckGLError();


	TransformComponent* cameraTransform = m_CameraEntity->GetComponent<TransformComponent>();
	glm::vec3 cameraPosition = cameraTransform->position;
	glm::vec3 cameraForward = cameraTransform->orientation * forward;

	glm::vec3 toOrigin = glm::normalize(-cameraPosition);
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, toOrigin, up);
	//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(-10.f, 0.f, 0.f), glm::vec3(-9.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(m_ViewMatrixUL, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	CheckGLError();

	std::vector<Entity*> entities;
	GetEntityManager().GetEntities(entities);
	
	for (int i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];
		if (entity->HasComponent<MeshRendererComponent>() && entity->HasComponent<TransformComponent>())
		{
			if (entity->isBullet && entity->inMotion)
			{

			}
			MeshRendererComponent* renderer = entity->GetComponent<MeshRendererComponent>();
			TransformComponent* transform = entity->GetComponent<TransformComponent>();

			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), transform->position);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), transform->scale);
			glm::mat4 rotationMatrix = glm::mat4_cast(transform->orientation);
			glm::mat4 modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;

			glUniformMatrix4fv(m_ModelMatrixUL, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			CheckGLError();

			glUniform3fv(m_ColorUL, 1, glm::value_ptr(renderer->color));
			CheckGLError();

			glBindVertexArray(renderer->vbo);
			CheckGLError();

			glDrawElements(GL_TRIANGLES, renderer->numTriangles * 3, GL_UNSIGNED_INT, (GLvoid*)0);
			CheckGLError();
		}
	}


	glutSwapBuffers();
}

void Engine::PressKey(unsigned char key)
{
	printf("%c down!\n", key);
	m_Keys[key] = true;
}

void Engine::ReleaseKey(unsigned char key)
{
	printf("%c up!\n", key);
	m_Keys[key] = false;
}

void Engine::PressSpecialKey(int key)
{
}

void Engine::ReleaseSpecialKey(int key)
{
}

void Engine::MouseMotion(int x, int y)
{
}

void Engine::MouseDrag(int x, int y)
{
}

void Engine::MouseButton(int button, int state)
{
}

void Engine::LoadAssets()
{
	// Load shader
	ShaderProgram simpleShader(
		"assets/shaders/SimpleShader.vertex.glsl",
		"assets/shaders/SimpleShader.fragment.glsl"
	);

	m_ShaderPrograms.push_back(simpleShader);
	CheckGLError();
	
	//
	// Move this:
	glUseProgram(simpleShader.id);
	CheckGLError();
	m_ProjectionMatrixUL = glGetUniformLocation(simpleShader.id, "ProjectionMatrix");
	m_ViewMatrixUL = glGetUniformLocation(simpleShader.id, "ViewMatrix");
	m_ModelMatrixUL = glGetUniformLocation(simpleShader.id, "ModelMatrix");
	m_ColorUL = glGetUniformLocation(simpleShader.id, "Color");
	CheckGLError();

	//
	// Meshes
	Model cone("assets/models/cone.obj");
	Model cube("assets/models/cube.obj");
	Model cylinder("assets/models/cylinder.obj");
	Model sphere("assets/models/sphere.obj");
	m_Models.push_back(cone);
	m_Models.push_back(cube);
	m_Models.push_back(cylinder);
	m_Models.push_back(sphere);

	//
	// Textures
	
	//
	// Entities
	const glm::vec3 origin(0.f);
	const glm::vec3 unscaled(1.f);
	const glm::quat identity(1.f, 0.f, 0.f, 0.f);

	//
	// Player #1
	m_Player = GetEntityManager().CreateEntity();
	m_Player->AddComponent<MeshRendererComponent>(cube.Vbo, cube.NumTriangles, glm::vec3(1.f, 0.f, 0.f));
	m_Player->AddComponent<TransformComponent>(glm::vec3(10.0,0.0,0.0), unscaled, identity);
	m_Player->AddComponent<PlayerControllerComponent>();
	m_Player->AddComponent<NetComponent>(true);

	Entity* bullet1 = GetEntityManager().CreateEntity();
	bullet1->AddComponent<MeshRendererComponent>(sphere.Vbo, sphere.NumTriangles, glm::vec3(0.3f, 0.3f, 0.3f));
	bullet1->AddComponent<TransformComponent>(glm::vec3(-30, 50, 0), unscaled, identity); //behind camera
	bullet1->parentOf = m_Player;
	bullet1->isBullet;
	bullet1->inMotion = false;
	m_NetworkedEntities.push_back(bullet1);

	// Create our player object
	/*Entity* player1 = GetEntityManager().CreateEntity();
	player1->AddComponent<MeshRendererComponent>(cube.Vbo, cube.NumTriangles, glm::vec3(0.f, 1.f, 0.f));
	player1->AddComponent<TransformComponent>(glm::vec3(20.0, 0.0, 0.0), unscaled, identity);
	player1->AddComponent<PlayerControllerComponent>();
	player1->AddComponent<NetComponent>(true);*/

	// Create our player object
	Entity* player2 = GetEntityManager().CreateEntity();
	player2->AddComponent<MeshRendererComponent>(cube.Vbo, cube.NumTriangles, glm::vec3(1.f, 1.f, 0.f));
	player2->AddComponent<TransformComponent>(glm::vec3(-10.0, 0.0, 0.0), unscaled, identity);
	player2->AddComponent<PlayerControllerComponent>();
	player2->AddComponent<NetComponent>(true);

	Entity* bullet2 = GetEntityManager().CreateEntity();
	bullet2->AddComponent<MeshRendererComponent>(sphere.Vbo, sphere.NumTriangles, glm::vec3(0.3f, 0.3f, 0.3f));
	bullet2->AddComponent<TransformComponent>(glm::vec3(-30, 50, 0), unscaled, identity); //behind camera
	bullet2->parentOf = player2;
	bullet2->isBullet;
	bullet2->inMotion = false;
	m_NetworkedEntities.push_back(bullet2);

	// Create our player object
	Entity* player3 = GetEntityManager().CreateEntity();
	player3->AddComponent<MeshRendererComponent>(cube.Vbo, cube.NumTriangles, glm::vec3(0.f, 0.f, 1.f));
	player3->AddComponent<TransformComponent>(glm::vec3(0.0, 0.0, 10.0), unscaled, identity);
	player3->AddComponent<PlayerControllerComponent>();
	player3->AddComponent<NetComponent>(true);

	Entity* bullet3 = GetEntityManager().CreateEntity();
	bullet3->AddComponent<MeshRendererComponent>(sphere.Vbo, sphere.NumTriangles, glm::vec3(0.3f, 0.3f, 0.3f));
	bullet3->AddComponent<TransformComponent>(glm::vec3(-30, 50, 0), unscaled, identity); //behind camera
	bullet3->parentOf = player3;
	bullet3->isBullet;
	bullet3->inMotion = false;
	m_NetworkedEntities.push_back(bullet3);

	// Create our player object
	Entity* player4 = GetEntityManager().CreateEntity();
	player4->AddComponent<MeshRendererComponent>(cube.Vbo, cube.NumTriangles, glm::vec3(0.f, 1.f, 1.f));
	player4->AddComponent<TransformComponent>(glm::vec3(0.0, 0.0, -10.0), unscaled, identity);
	player4->AddComponent<PlayerControllerComponent>();
	player4->AddComponent<NetComponent>(true);

	Entity* bullet4 = GetEntityManager().CreateEntity();
	bullet4->AddComponent<MeshRendererComponent>(sphere.Vbo, sphere.NumTriangles, glm::vec3(0.3f, 0.3f, 0.3f));
	bullet4->AddComponent<TransformComponent>(glm::vec3(-30, 50, 0), unscaled, identity); //behind camera
	bullet4->parentOf = player4;
	bullet4->isBullet;
	bullet4->inMotion = false;
	m_NetworkedEntities.push_back(bullet4);


	// Camera
	m_CameraEntity = GetEntityManager().CreateEntity();
	glm::quat rotation = identity * glm::vec3(0.f, -1.f, -0.1f);
	m_CameraEntity->AddComponent<TransformComponent>(glm::vec3(-20.f, 50.f, 0.f), unscaled, rotation);
}

void Engine::AddSystem(System* system)
{
	m_Systems.push_back(system);
}
gdpNamespaceEnd