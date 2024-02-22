#pragma once

#include <common.h>
#include <chrono>

#include <gameplay/gameworld.h>

#include <graphics/model.h>
#include <graphics/shaderprogram.h>

#include <network/networkmanager.h>

#include <system/system.h>

gdpNamespaceBegin
void _CheckGLError(const char* file, int line);

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)
// Move this stuff

class Engine
{
public:
	Engine();
	~Engine();

	//
	// Lifecycle
	void Initialize();
	void Destroy();

	//
	// Window
	void Resize(int w, int h);

	//
	// Update
	void Update();

	//
	// Render
	void Render();

	//
	// Keyboard 
	void PressKey(unsigned char key);
	void ReleaseKey(unsigned char key);
	void PressSpecialKey(int key);
	void ReleaseSpecialKey(int key);

	//
	// Mouse
	void MouseMotion(int x, int y);
	void MouseDrag(int x, int y);
	void MouseButton(int button, int state);

	//
	// Engine
	void AddSystem(System* system);

private:
	void LoadAssets();

	bool m_Initialized = false;

	//
	// Systems
	std::vector<System*> m_Systems;

	//
	// Window information
	int m_WindowX = 0;
	int m_WindowY = 0;
	int m_WindowWidth = 0;
	int m_WindowHeight = 0;
	int m_WindowRatio = 0;

	//
	// Model stuff
	std::vector<Model> m_Models;

	// 
	// Shader stuff
	std::vector<ShaderProgram> m_ShaderPrograms;
	GLuint m_ViewMatrixUL;
	GLuint m_ModelMatrixUL;
	GLuint m_ProjectionMatrixUL;
	GLuint m_ColorUL;

	// 
	// NetworkManager
	net::NetworkManager m_NetworkManager;

	//
	// Input (MOVE THIS)
	bool m_Keys[255];

	//
	// GameWorld
	GameWorld* m_GameWorld;
	Entity* m_Player;
	std::vector<Entity*> m_NetworkedEntities;

	//
	// Camera
	Entity* m_CameraEntity;

	// 
	// Time
	std::chrono::high_resolution_clock::time_point m_LastTime;
};

gdpNamespaceEnd